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

/**

 Very basic test for the FastFlow farm.
 
*/
#include <vector>
#include <iostream>
#include <ff/PEI/DSRIManagerNode.hpp>

using namespace ff;

// generic worker
class Worker: public ff_node {
public:
    void * svc(void * task) {
        int * t = (int *)task;
        //std::cout << "Worker " << ff_node::get_my_id() 
          //       << " received task " << *t << "\n";
        return task;
    }
    void svc_end(){
       std::cout << "worker: "<< get_node_address()<< std::endl;
    }
 
};

// the gatherer filter
class Collector: public ff_node {
public:
    void * svc(void * task) {
        int * t = (int *)task;
        if (*t == -1) return NULL;
        return task;
    }
    void svc_end(){
        std::cout << "collector: " << get_node_address()<< std::endl;
    }
};

// the load-balancer filter
class Emitter: public ff_node {
public:
    Emitter(int max_task):ntask(max_task) {};

    void * svc(void *) {	
        int * task = new int(ntask);
        --ntask;
        if (ntask<0) return NULL;
	 ff_send_out((void*)(task)); // to use static scheduler it must be ff_send_out otherewise it does not work
        return GO_ON;
    }
    void svc_end(){
      //  std::cout << "emitter: "<<get_node_address()<< std::endl;
    }
private:
    int ntask;
};
class compFactory:public componentFactory{
public:
    compFactory(unsigned int theLength):theLength(theLength){}

    ff_node* InstantiateComponents(std::string name){
        if (name.compare("Emitter")==0){
            return (new Emitter(theLength));
        }else if (name.compare("Collector")==0){
            return (new Collector());
        }else if (name.compare("Worker")==0){
            return (new Worker());
        }
        return NULL;
    }
private: 
    unsigned int theLength;  
};

int main(int argc, char * argv[]) {

    if (argc<2) {
        std::cerr << "use: " << argv[0] << " streamlen\n";
        return -1;
    }
    
    int streamlen=atoi(argv[1]);

    if (!streamlen) {
        std::cerr << "Wrong parameters values\n";
        return -1;
    } 
    compFactory *cmp_f = new compFactory(streamlen);
    ff_manager  m_ffManager;
    ff_node* farm =m_ffManager.generate_skeleton_tree("/home/mehdi/Dropbox/FastFlow_dev/fastflow-2.0.3-VIP/tests/builder_test/JSONFile/build_farm.json", cmp_f);
    farm->run_and_wait_end();
    m_ffManager.finalise();
    
    return 0;
}
