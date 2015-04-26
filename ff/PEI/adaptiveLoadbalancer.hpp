/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */

/*!
 * \link 
 * \file clEnvironment.hpp
 * \ingroup opencl_fastflow
 *
 * \brief TODO
 */

/* ***************************************************************************
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as 
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *  As a special exception, you may use this file as part of a free software
 *  library without restriction.  Specifically, if other files instantiate
 *  templates or use macros or inline functions from this file, or you compile
 *  this file and link it with other files to produce an executable, this
 *  file does not by itself cause the resulting executable to be covered by
 *  the GNU General Public License.  This exception does not however
 *  invalidate any other reasons why the executable file might be covered by
 *  the GNU General Public License.
 *
 ****************************************************************************
 Mehdi Goli: goli.mehdi@gmail.com*/
 
#include <ff/lb.hpp>
#include <vector>
#include <ctime>
//#include <cmath>
//using namespace ff;
namespace ff {

class loadBound{
public:
  loadBound(double min, double max): min(min), max(max){}
  double min;
  double max;
};
class adaptive_loadbalancer: public ff_loadbalancer{

protected:

  // implement your policy...
  inline int selectworker() {
       int victim=set_victim();
    //   if(victim!=-1){
     //  if(no_scheduled_task!=0 && (no_scheduled_task%(no_active_worker))==0) {
       //	waitCall(t_sec, t_milisec);
       	//std::cout << "Time to sleep"<<t_sec << "\t" << t_milisec <<std::endl;
       //}
     //  ++no_scheduled_task;}
       // if(victim>=0) std::cout<<"selectedworker "<< victim<< std::endl;
       return (victim>=0 ? (victim):(ff_loadbalancer::selectworker()));
  }
public:
    inline int get_worker_queue_bound(int ondemand, int i) { 
      return( (i<queuebounds.size() && queuebounds.at(i)>0 ) ? queuebounds.at(i):ondemand );
    }
    inline void set_worker_queue_bound(std::vector<int> v)
    {
        queuebounds.swap(v);    
    }
  // this is necessary because ff_loadbalancer has non default parameters....
  adaptive_loadbalancer(int max_num_workers):ff_loadbalancer(max_num_workers){//flag=false;
     srandom(::getpid()+(getusec()%4999));
     pthread_mutex_init(&lbLoadChange, NULL);
    // no_scheduled_task=0;
     no_active_worker=0;
     arrRate=0;

  }
  //virtual double getArrivalRate(){return arrRate;}
  
  //  throtler= new bool[ntentative()]();
    // for(int i=0; i<ntentative(); i++) {std::cout<<"val"<< i << ":"<< throtler[i]<<std::endl;}
  
  virtual JSONNode get_sensors(){
    JSONNode sensors=ff_loadbalancer::get_sensors(); 
    sensors.at_nocase("Ch_Policy")="adaptive_loadbalancer";
    return sensors;

  }

  int set_victim() { // implement your own scheduler whatever you want
    if( pthread_mutex_trylock(&lbLoadChange)==0){
      if(boundaries.size()==0) {
	pthread_mutex_unlock(&lbLoadChange);
	return -1;
      }
      int victim=-1;
      double ld = ((double) rand() / (RAND_MAX));
      for(int i=0; i<boundaries.size(); i++){
		if((ld>=boundaries.at(i)->min) && (ld< boundaries.at(i)->max)){
	  		victim= i;
	  		pthread_mutex_unlock(&lbLoadChange);
	  		return victim;
		}
      }
      pthread_mutex_unlock(&lbLoadChange);
      return victim;
    }
    return -1;
}
void setWorkLoad(std::vector<double> workload , double arr_rate){
    std::cout<< "loading the workload"<<std::endl;
  std::vector<loadBound*> calculatedboundaries;
  double min =0.0;
  int count_no_active_worker=0;
  for (std::vector<double>::iterator it = workload.begin() ; it != workload.end(); ++it){
    if((*it)!=0.0){
    	count_no_active_worker++;
    	calculatedboundaries.push_back((new loadBound(min, (min + (*it)))));
    	min += (*it);
    }else{
      calculatedboundaries.push_back((new loadBound(0, 0)));
    }
  }
  no_active_worker=count_no_active_worker;
  arrRate=arr_rate;
  //t_milisec = fmod (arrRate,1000);
  //t_sec = ((arrRate- t_milisec)/1000);
  if(!(calculatedboundaries.empty())){
    pthread_mutex_lock(&lbLoadChange);
    boundaries.swap(calculatedboundaries);
    pthread_mutex_unlock(&lbLoadChange);
  }
}

private:
  std:: vector<loadBound*> boundaries;
  pthread_mutex_t lbLoadChange;
  double arrRate; // at the moment it is potentially added by extractig from the code but i dont use the provided arrival rate by libjson_utilities function for now.
  int no_active_worker;
  std::vector<int> queuebounds;
 // long long no_scheduled_task;
  //double t_sec; 
  //double t_milisec;
  //bool * throtler;
};
}

