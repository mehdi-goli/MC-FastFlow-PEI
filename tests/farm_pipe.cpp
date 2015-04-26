/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
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
 */

/*
 * Very basic test for the FastFlow pipeline (actually a 2-stage torus).
 *
 */
#include <iostream>
#include <ff/pipeline.hpp>
#include <ff/farm.hpp>
//#include <ff/PEI/DSRIManagerNode.hpp>

using namespace ff;


class Emitter: public ff_node {
public:
    Emitter( int max_task):max_task(max_task){
        ntask=0;
    }

    void * svc(void *) {    
       int* t;
      if (ntask == max_task) {
         std::cout << "ntask" << ntask <<"\n";
        return NULL;}
      else{
       int* t = ( int*)malloc(sizeof(int));
        if (!t) abort();
        *t=1;
        ntask++;
        ff_send_out((void*)(t)); // to use static scheduler it must be ff_send_out otherewise it does not work
        return GO_ON;
    }
}
private:
    int ntask;
    int max_task;
};


// generic stage
class worker: public ff_node {
public:
    worker():sum(0)  {}

    void * svc(void * task) {
       // return task;
        unsigned int * t = (unsigned int *)task;

      if(*t==-1){
         //std::cout << "t is NULL" << "\n";
        return NULL;
    }else{
        *t = (*t)+sum;
        sum=*t;
        task = t;
        return task;}
    }
    void  svc_end() {
            std::cout << "Sum Worker: " << ff_node::get_my_id() << " : "<<sum << "\n";
    }

private:
     int sum;
};



class Stage: public ff_node {
public:
    Stage():sum(0)  {}

    void * svc(void * task) {
       // return task; 
           int * t = ( int *)task; 
      if(*t==-1){
        return NULL;
    }else{
        std::cout<<"recieved:"<<*t<<std::endl;
        sum+=*t;
       task = t;
        return task;}
    }
    void  svc_end() {
       if (ff_node::get_my_id()) 
            std::cout << "Sum Stage: " << sum << "\n";
    }

private:
     int sum;
};



class Collector: public ff_node {
public:
    void * svc(void * task) {
        int * t = (int *)task;
        if (*t ==-1 ) return NULL;
        return task;
    }
};


int main(int argc, char * argv[]) {
    if (argc!=2) {
        std::cerr << "use: "  << argv[0] << " streamlen\n";
        return -1;
    }
    
    // build a 2-stage pipeline
    ff_pipeline pipe;
    ff_farm<> farm;
    Emitter e(atoi(argv[1]));
    farm.add_emitter(&e);
    std::vector<ff_node*> v;
    for (int i=0; i<4; i++){
       v.push_back(new worker());
    }
    farm.add_workers(v);
    Collector c;
    farm.add_collector(&c); 
    //Stage * st1=new Stage(atoi(argv[1]));
    //pipe.add_stage(st1);
    pipe.add_stage(&farm);
    Stage * st2=new Stage();
    pipe.add_stage(st2);

   // pipe.wrap_around();

    ffTime(START_TIME);

  //  PROFILE(&pipe);
    if (pipe.run_and_wait_end()<0) {
        error("running pipeline\n");
        return -1;
    }
    ffTime(STOP_TIME);

    std::cerr << "DONE, pipe  time= " << pipe.ffTime() << " (ms)\n";
    std::cerr << "DONE, total time= " << ffTime(GET_TIME) << " (ms)\n";
    pipe.ffStats(std::cerr);
    return 0;
}
