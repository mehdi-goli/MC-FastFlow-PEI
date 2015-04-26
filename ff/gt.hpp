/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */

/*! 
 *  \file gt.hpp
 *  \ingroup streaming_network_arbitrary_shared_memory
 *
 *  \brief It Contains the \p ff_gatherer class and methods which are used to model the \a
 *  Collector node, which is optionally used to gather tasks coming from
 *  workers.
 */

#ifndef _FF_GT_HPP_
#define _FF_GT_HPP_
/* ***************************************************************************
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License version 3 as 
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *
 ****************************************************************************
 */

#include <iostream>
#include <deque>
#include <ff/svector.hpp>
#include <ff/utils.hpp>
#include <ff/node.hpp>

namespace ff {

/*!
 *  \ingroup streaming_network_arbitrary_shared_memory
 *
 *  @{
 */

/*!
 *  \class ff_gatherer
 *  \ingroup streaming_network_arbitrary_shared_memory
 *
 *  \brief A class representing the \a Collector node in a \a Farm skeleton.
 *
 *  This class models the \p gatherer, which wraps all the methods and
 *  structures used by the \a Collector node in a \p Farm skeleton. The \p farm
 *  can be seen as a three-stages \p pipeline, the stages being a \p
 *  ff_loadbalancer called \a emitter, a pool of \p ff_node called \a workers
 *  and - optionally - a \p ff_gatherer called \a collector. The \a Collector
 *  node can be used to gather the results coming from the computations
 *  executed by the pool of \a workers. The \a collector can also be
 *  connected to the \a emitter node via a feedback channel, in order to create
 *  a \p farm-with-feedback skeleton.
 *
 *  This class is defined in \ref gt.hpp
 *
 */

class ff_gatherer: public ff_thread {

    template <typename T1, typename T2>  friend class ff_farm;
    friend class ff_pipeline;
public:
    /*Mehdi:FF-VIP*/
   // virtual void registerEndCallback(void(*cb)(void*), void *param=NULL) {
     //   assert(cb != NULL);
       // end_callback=cb;
       // end_callback_param = param;
   // }
    enum {TICKS2WAIT=5000};

protected:

    /**
     * \brief Selects a worker.
     * 
     * It gets the next worker using the Round Robin policy. The selected
     * worker has to be alive (and kicking).
     *
     * \return The next worker to be selected.
     *
     */
    virtual inline int selectworker() { 
        do nextr = (nextr+1) % nworkers;
        while(offline[nextr]);
        return nextr;
    }

    /**
     * \brief Notifies the EOS
     *
     * It is a virtual function and is used to notify EOS
     */
    virtual inline void notifyeos(int id) {}

    /**
     * \brief Gets the number of tentatives.
     *
     * The number of tentative before wasting some times and than retry 
     */
    virtual inline unsigned int ntentative() { return nworkers;}

    /**
     * \brief Loses the time out.
     *
     * It is a virutal function which defines the number of ticks to be waited.
     *
     */
    virtual inline void losetime_out() { 
        FFTRACE(lostpushticks+=TICKS2WAIT;++pushwait);
        ticks_wait(TICKS2WAIT); 
    }
    /**
     * \brief Loses the time in 
     *
     * It is a virutal function which defines the number of ticks to be waited.
     *
     */
    virtual inline void losetime_in() { 
        FFTRACE(lostpopticks+=TICKS2WAIT;++popwait);
        ticks_wait(TICKS2WAIT);
    }    

    /**
     * \brief It gathers the tasks.
     *
     * It keeps selecting the worker. If a worker has task, then the worker is
     * returned. Otherwise a tick is wasted and then keep looking for the
     * worker with the task.
     *
     * \return It returns the workers with a taks if successful. Otherwise -1
     * is returned.
     */
    virtual int gather_task(void ** task) {
        register unsigned int cnt;
        do {
            cnt=0;
            do {
                nextr = selectworker();
                assert(offline[nextr]==false);
                if (workers[nextr]->get(task)) return nextr;
                else if (++cnt == ntentative()) break;
            } while(1);
            losetime_in();
        } while(1);

        // not reached
        return -1;
    }


    /**
     *
     * \brief It gathers all tasks.
     *
     * It is a virtual function, and gathers results from the workers. 
     *
     * \return It returns 0 if the tasks from all the workers are collected.
     * Otherwise a negative value is returned.
     *
     */
    virtual int all_gather(void *task, void **V) {
        V[channelid]=task;
        int nw=getnworkers();
        svector<ff_node*> _workers(nw);
        for(int i=0;i<nworkers;++i) 
            if (!offline[i]) _workers.push_back(workers[i]);
        svector<int> retry(nw);

        for(register int i=0;i<nw;++i) {
            if(i!=channelid && !_workers[i]->get(&V[i]))
                retry.push_back(i);
        }
        while(retry.size()) {
            channelid = retry.back();
            if(_workers[channelid]->get(&V[channelid]))
                retry.pop_back();
            else losetime_in();
        }
        for(register int i=0;i<nw;++i)
            if (V[i] == (void *)FF_EOS || V[i] == (void*)FF_EOS_NOFREEZE)
                return -1;
        FFTRACE(taskcnt+=nw-1);
        return 0;
    }

    /**
     * \brief Pushes the task in the tasks queue.
     *
     * It pushes the tasks in a queue. 
     */

    void push(void * task) {
        //register int cnt = 0;
        if (!filter) {
            while (! buffer->push(task)) {
                // if (ch->thxcore>1) {
                // if (++cnt>PUSH_POP_CNT) { sched_yield(); cnt=0;}
                //    else ticks_wait(TICKS2WAIT);
                //} else 
                losetime_out();
            }     
            return;
        }

        while (! filter->push(task)) {
            // if (ch->thxcore>1) {
            // if (++cnt>PUSH_POP_CNT) { sched_yield(); cnt=0;}
            //    else ticks_wait(TICKS2WAIT);
            //} else 
            losetime_out();
        }
    }

    /**
     * \brief Pop a task out of the queue.
     *
     * It pops the task out of the queue.
     *
     * \return \p false if not successful, otherwise \p true is returned.
     *
     */
    bool pop(void ** task) {
        //register int cnt = 0;       
        if (!get_out_buffer()) return false;
        while (! buffer->pop(task)) {
            losetime_in();
        } 
        return true;
    }

    /**
     * \brief Pop a tak from un unbounded queue.
     *
     * It pops the task from an unbounded queue.
     *
     * \return The task popped from the buffer.
     */
    bool pop_nb(void ** task) {
        if (!get_out_buffer()) return false;
        return buffer->pop(task);
    }

public:

    /**
     *  \brief Constructor
     *
     *  It creates \p max_num_workers and \p NULL pointers to worker objects.
     */
    ff_gatherer(int max_num_workers):
        max_nworkers(max_num_workers),nworkers(0),nextr(0),
        neos(0),neosnofreeze(0),channelid(-1),
        filter(NULL), workers(max_nworkers), offline(max_nworkers), buffer(NULL),
        skip1pop(false),/*end_callback(NULL), end_callback_param(NULL),*/EXIT(false) {
        time_setzero(tstart);time_setzero(tstop);
        time_setzero(wtstart);time_setzero(wtstop);
        wttime=0;
        active_time=0;
        suspend=false;
        last_time=0;
        FFTRACE(taskcnt=0;lostpushticks=0;pushwait=0;lostpopticks=0;popwait=0;ticksmin=(ticks)-1;ticksmax=0;tickstot=0);
        /*Mehdi:FF-VIP*/
        pthread_mutex_init(&exitLock, NULL);
    }

    virtual ~ff_gatherer() {
        /*Mehdi:FF-VIP*/ // this should be there for profiling collector
        if(filter)
            if (filter->end_callback) 
                filter->end_callback(filter->end_callback_param);
    }

    /**
     * \brief Sets the filer
     *
     * It sents the \p ff_node to the filter.
     *
     * \return 0 if successful, otherwise a negative value is returned.
     */
    int set_filter(ff_node * f) { 
        if (filter) {
            error("GT, setting collector filter\n");
            return -1;
        }
        filter = f;
        return 0;
    }

    /**
     * \brief Sets output buffer
     *
     * It sets the output buffer.
     */
    void set_out_buffer(FFBUFFER * const buff) { buffer=buff;}

    /**
     * \brief Gets the channel id
     *
     * It gets the \p channelid.
     *
     * \return The \p channelid is returned.
     */
    int get_channel_id() const { return channelid;}

    /**
     * \brief Gets the number of worker threads currently running.
     *
     * It gets the number of threads currently running.
     *
     * \return Number of worker threads
     */
    inline int getnworkers() const { return (int) workers.size()-neos-neosnofreeze; }
    
    /**
     * \brief Skips the first pop
     *
     * It determine whether the first pop should be skipped or not.
     *
     * \return Always \true is returned.
     */
    void skipfirstpop() { skip1pop=true; }

    /**
     * \brief Gets the ouput buffer
     *
     * It gets the output buffer
     *
     * \return \p buffer is returned. 
     */
    FFBUFFER * get_out_buffer() const { return buffer;}

    /**
     * \brief Register the given worker to the list of workers.
     *
     * It registers the given worker to the list of workers.
     *
     * \return 0 if successful, or -1 if not successful.
     */
    int  register_worker(ff_node * w) {
        if (nworkers>=max_nworkers) {
            error("GT, max number of workers reached (max=%d)\n",max_nworkers);
            return -1;
        }
        workers.push_back(w);
        ++nworkers;
        return 0;
    }

    /**
     * \brief Initializes the gatherer task.
     *
     * It is a virtual function to initialise the gatherer task.
     *
     * \return It returns the task if successful, otherwise 0 is returned.
     */
    virtual int svc_init() { 
        gettimeofday(&tstart,NULL);
        for(unsigned i=0;i<workers.size();++i)  offline[i]=false;
        if (filter) return filter->svc_init(); 
        return 0;
    }

    /**
     * \brief The gatherer task
     *
     * It is a virtual function to be used as the gatherer task.
     *
     * \return It returns the task.
     */
    virtual void * svc(void *) {
        void * ret  = (void*)FF_EOS;
        void * task = NULL;
        bool outpresent  = (get_out_buffer() != NULL);
        bool skipfirstpop = skip1pop;

        // the following case is possible when the collector is a dnode
        if (!outpresent && filter && (filter->get_out_buffer()!=NULL)) {
            outpresent=true;
            set_out_buffer(filter->get_in_buffer());
        }
       
        gettimeofday(&wtstart,NULL);
        do {
            
            if(suspend)
                suspend_node();

            task = NULL;
            if (!skipfirstpop) 
                nextr = gather_task(&task); 
            else skipfirstpop=false;

            if (task == (void *)FF_EOS) {
                if (filter) filter->eosnotify(workers[nextr]->get_my_id());
                offline[nextr]=true;
                ++neos;
                ret=task;
            } else if (task == (void *)FF_EOS_NOFREEZE) {
                if (filter) filter->eosnotify(workers[nextr]->get_my_id());
                offline[nextr]=true;
                ++neosnofreeze;
                ret = task;
            } else {
                FFTRACE(++taskcnt);
                if (filter)  {
                    channelid = workers[nextr]->get_my_id();
                    FFTRACE(register ticks t0 = getticks());
                    double time0= getElapsedTime();
                    task = filter->svc(task);
                    active_time+=getElapsedTime()-time0;
                    last_time=getElapsedTime()-time0;

#if defined(TRACE_FASTFLOW)
                    register ticks diff=(getticks()-t0);
                    /*Mehdi:FF-VIP*/
                    if (task && (task != (void*)FF_EOS) && (task != (void*)FF_EOS_NOFREEZE)) { 
                        if((filter->get_modular_mode()!=0)&&((taskcnt)!=0)&&(((taskcnt)%(filter->get_modular_mode()))==0)){
                            if( pthread_mutex_trylock(&(filter->readWriteProfile))==0){
                                filter->svcTicksInterval.push_back(diff);
                                pthread_mutex_unlock(&(filter->readWriteProfile));
                            }
                        }
                    }/*Mehdi:FF-VIP*/
                    tickstot +=diff;
                    ticksmin=(std::min)(ticksmin,diff);
                    ticksmax=(std::max)(ticksmax,diff);
#endif    
                }

                // if the filter returns NULL we exit immediatly
                if (task == GO_ON) continue;
                
                // if the filter returns NULL we exit immediatly
                if (task ==(void*)FF_EOS_NOFREEZE) { 
                    ret = task;
                    break; 
                }
                if (!task || task==(void*)FF_EOS) {
                    ret = (void*)FF_EOS;
                    break;
                }                
                if (outpresent) {
                    //if (filter) filter->push(task);
                    //else 
                    push(task);
                }
            }
        } while((neos<nworkers) && (neosnofreeze<nworkers));

        if (outpresent) {
            // push EOS
            task = ret;
            push(task);
        }
        /*Mehdi:FF-VIP*/
        if(filter){
            filter->setEXIT(true);
        }else{
            setEXIT(true);
        }
        gettimeofday(&wtstop,NULL);
        wttime+=diffmsec(wtstop,wtstart);
        if (neos>=nworkers) neos=0;
        if (neosnofreeze>=nworkers) neosnofreeze=0;

        return ret;
    }

    /**
     * \brief Finializes the gatherer.
     *
     * It is a virtual function used to finalise the gatherer task.
     *
     */
    virtual void svc_end() {
        if (filter) filter->svc_end();
        gettimeofday(&tstop,NULL);
    }

    /**
     * \brief Execute the gatherer task.
     *
     * It executes the gatherer task.
     *
     * \return 0 if successful, otherwise -1 is returned.
     */
    int run(bool=false) {  
        if (this->spawn(filter?filter->getCPUId():-1)== -2) {
            error("GT, spawning GT thread\n");
            return -1; 
        }
        return 0;
    }

    /**
     * \brief Start counting time
     *
     * It defines the counting of start time.
     *
     * \return Difference in milli seconds.
     */
    virtual double ffTime() {
        return diffmsec(tstop,tstart);
    }

    // Mehdi
    virtual double getElapsedTime(){
        struct timeval tcurrent;
        gettimeofday(&tcurrent, NULL);
        return diffmsec(tcurrent,tstart);
    }
    //Mehdi

    /**
     * \brief Complete counting time
     *
     * It defines the counting of finished time.
     *
     * \return Difference in milli seconds.
     */
    virtual double wffTime() {
        return diffmsec(wtstop,wtstart);
    }
   

    virtual const struct timeval & getstarttime() const { return tstart;}
    virtual const struct timeval & getstoptime()  const { return tstop;}
    virtual const struct timeval & getwstartime() const { return wtstart;}
    virtual const struct timeval & getwstoptime() const { return wtstop;} 

    /*Mehdi:FF_VIP*/
    virtual void set_actuators(JSONNode actuators){
        if(filter && actuators.type()!=JSON_NULL)
        {
            JSONNode::const_iterator iter = actuators.begin();
            if(strcasecmp(iter->name().c_str(), "Action") == 0 && strcasecmp(iter->as_string().c_str(), "Suspend") == 0 )
                suspend=true;
            else if(strcasecmp(iter->name().c_str(), "Action") == 0 && strcasecmp(iter->as_string().c_str(), "Resume") == 0 )
            {
                suspend=false;
                resume_node();
            }
            else
            {
              //  JSONNode::const_iterator it =actuators.find_nocase("Assigned_Processor");
               // if(it!=actuators.end() && it->as_int()!=-1) {filter->setAffinity(it->as_int());}
            }
        }
    }
    /*Mehdi:FF_VIP*/
    /*virtual void actuator(mObject instObj){
        if(filter){
            int pId;
            mValue mv =find_value(instObj, "Assigned_Processor");
            if(!(mv.is_null())) {pId= mv.get_int();}
            if(pId!=-1){filter->setAffinity(pId);}
        }
    }*/
    /*Mehdi:FF_VIP*/
    void setDataSize(unsigned long long dataSize){
        if(filter){filter->setDataSize(dataSize);}
    }
    /*Mehdi:FF_VIP*/
    virtual bool getEXIT(){
        if(filter) {return filter->getEXIT();}
        else{
            pthread_mutex_lock(&(this->exitLock));
            bool ex =this->EXIT;
            pthread_mutex_unlock(&(this->exitLock));
            return ex;
        }
    }
    /*Mehdi:FF_VIP*/
    void setEXIT(bool val){ 
        if (filter){filter->setEXIT(val);}
        else{
            pthread_mutex_lock(&(this->exitLock));
            this->EXIT=val;
            pthread_mutex_unlock(&(this->exitLock));
        }
    }

#if defined(TRACE_FASTFLOW)  
    /**
     * \brief The trace of FastFlow
     *
     * It prints the trace for FastFlow.
     *
     */
    virtual void ffStats(std::ostream & out) { 
        out << "Collector: "
            << "  work-time (ms): " << wttime    << "\n"
            << "  n. tasks      : " << taskcnt   << "\n"
            << "  svc ticks     : " << tickstot  << " (min= " << (filter?ticksmin:0) << " max= " << ticksmax << ")\n"
            << "  n. push lost  : " << pushwait  << " (ticks=" << lostpushticks << ")" << "\n"
            << "  n. pop lost   : " << popwait   << " (ticks=" << lostpopticks  << ")" << "\n";
    }

      /*Mehdi:FF-VIP*/
    void set_parent_name(std::string pname){
        if(filter)
        {
            filter->set_parent_name(pname);
        }
    }
    virtual JSONNode get_sensors(){
        JSONNode sensors(JSON_NODE);
        sensors.set_name("NTo1Com");
        int status;
        sensors.push_back(JSONNode("Component_Name", abi::__cxa_demangle(typeid(*this).name(), 0,0,&status )));
        sensors.push_back(JSONNode("Ch_Policy", "ff_gatherer"));
        sensors.push_back(JSONNode("Processed_Tasks", taskcnt));
        sensors.push_back(JSONNode("Elapsed_Time", getElapsedTime()));
        bool outpresent  = (get_out_buffer() != NULL);
        outpresent? sensors.push_back(JSONNode("Queue_Output", buffer->length())):
         sensors.push_back(JSONNode("Queue_Output", -1));
        if(filter){
            sensors.push_back(JSONNode("Component_Type", "reduce"));
            JSONNode jn_filter(JSON_NODE);
            jn_filter.set_name("Filter");
            jn_filter.push_back(JSONNode("Component_Name", abi::__cxa_demangle(typeid(*filter).name(), 0,0,&status )));
            jn_filter.push_back(JSONNode("Component_Type", "ff_node"));
            sensors.push_back(JSONNode("Node_Address", filter->get_node_address()));
            std::vector<ticks> collectedSamples;
            pthread_mutex_lock(&(filter->readWriteProfile));
            collectedSamples.swap((filter->svcTicksInterval));
            pthread_mutex_unlock(&(filter->readWriteProfile));
            sensors.push_back(JSONNode("Assigned_Processor", filter->getCPUId()));
            //jn_filter.push_back(JSONNode("Data_Size", filter->dataSize));
            filter->get_modular_mode()!=0 ? sensors.push_back(JSONNode("Sampling_Rate", (1.0/filter->get_modular_mode()))) : sensors.push_back(JSONNode("Sampling_Rate", 0));
            jn_filter.push_back(filter->getSamples(filter->dataSize,collectedSamples,"Component_Time_Distribution"));
            sensors.push_back(jn_filter);
        }else{
            sensors.push_back(JSONNode("Component_Type", "g-pol"));
            sensors.push_back(JSONNode("Assigned_Processor", -1));
        }
        //sensors.push_back(JSONNode("Total_SVC_Ticks", tickstot));
        //sensors.push_back(JSONNode("Total_SVC_Time", wttime));
        sensors.push_back(JSONNode("Component_Last_Processing_Time", last_time));
        sensors.push_back(JSONNode("Total_Component_Active_Time", active_time));
        //sensors.push_back(JSONNode("Minimum_SVC_Tick", ticksmin));
        //sensors.push_back(JSONNode("Maximum_SVC_Tick", ticksmax));
        sensors.push_back(JSONNode("Push_Delay_Count", pushwait));
        double p_time = static_cast<double>((static_cast<double>(lostpushticks))/CLOCKS_PER_SEC);
        sensors.push_back(JSONNode("Push_Delay_Time", p_time));
        sensors.push_back(JSONNode("Pop_Delay_Count", popwait));
        p_time = static_cast<double>((static_cast<double>(lostpopticks))/CLOCKS_PER_SEC);
        sensors.push_back(JSONNode("Pop_Delay_Time", p_time));
        return sensors;
    }

    /*Mehdi:FF_VIP*/
    /*virtual mObject sensor(){
        mObject gtObj;
        int status;
        std::vector<ticks> collectedSamples;
        gtObj["FF::Node_Subclass"]=abi::__cxa_demangle(typeid(*this).name(), 0,0,&status );
        gtObj["Type"]="FF::GATHERER";
        gtObj["Total_Number_Of_Tasks"]= taskcnt;
        if(filter){
            pthread_mutex_lock(&(filter->readWriteProfile));
            collectedSamples.swap((filter->svcTicksInterval));
            pthread_mutex_unlock(&(filter->readWriteProfile));
            gtObj["Assigned_Processor"]=filter->getCPUId();
            gtObj["Data_Size"]= filter->dataSize;
            gtObj["Total_SVC_Ticks"]= static_cast<uint64_t>(tickstot);
            gtObj["Minimum_SVC_Ticks"]= static_cast<uint64_t>(ticksmin);
            gtObj["Maximum_SVC_Ticks"]= static_cast<uint64_t>(ticksmax);
            if(filter->get_modular_mode()!=0)
                gtObj["Sampling_Rate"]= 1.0/filter->get_modular_mode();
            else
                gtObj["Sampling_Rate"]=0;
            gtObj["Sampled_SVC_Tick_Distribution"]= getSamples(filter->dataSize,collectedSamples);
        }else
            gtObj["Assigned_Processor"]=-1; 
        gtObj["Push_Delay_Count"]=pushwait;
        gtObj["Push_Delay_Ticks"]= static_cast<uint64_t>(lostpushticks);
        gtObj["Pop_Delay_Count"]= popwait;
        gtObj["Pop_Delay_Ticks"]= static_cast<uint64_t>(lostpopticks);
        return gtObj;
    };*/
    /*Mehdi:FF_VIP*/
    void set_modular_mode(int val){
        if(filter){filter->set_modular_mode(val);}
    }
#endif

private:
    int               max_nworkers;
    int               nworkers; // this is the # of workers initially registered
    int               nextr;

    int               neos;
    int               neosnofreeze;
    int               channelid;

    ff_node         * filter;
    svector<ff_node*> workers;
    svector<bool>     offline;
    FFBUFFER        * buffer;
    bool              skip1pop;

    struct timeval tstart;
    struct timeval tstop;
    struct timeval wtstart;
    struct timeval wtstop;
    double wttime;
    double active_time;
    /*Mehdi:FF_VIP*/
  //  void (*end_callback)(void*);  
    /*Mehdi:FF_VIP*/
   // void *end_callback_param;
    /*Mehdi:FF_VIP*/
    unsigned long recieved_task;
    bool suspend;
    double last_time;
    /*Mehdi:FF_VIP*/
    bool EXIT;
    /*Mehdi:FF_VIP*/
    pthread_mutex_t exitLock;
#if defined(TRACE_FASTFLOW)
    unsigned long taskcnt;
    ticks         lostpushticks;
    unsigned long pushwait;
    ticks         lostpopticks;
    unsigned long popwait;
    ticks         ticksmin;
    ticks         ticksmax;
    ticks         tickstot;
#endif
};

/*!
 *  @}
 *  \endlink
 *
 */

} // namespace ff

#endif /*  _FF_GT_HPP_ */
