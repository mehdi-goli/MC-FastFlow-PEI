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
Mehdi Goli: goli.mehdi@gmail.com */
/*
 This is the convolution application that does not use PEI for performance enhancement
 */
#include "examples/Image_processing/Funcs/functions.cpp"
#include <ff/farm.hpp>
#include <ff/pipeline.hpp>
#if defined(FASTFLOW_OPENCL)
	#include <ff/oclnode.hpp>
#else
	#include <ff/node.hpp>
#endif
#include <sstream>      // std::ostringstream
#include <map>
using namespace ff;


  /*emitter*/
class emitter_cls: public ff_node { 
public:
	emitter_cls(){}

	int svc_init(){
	
		return 0;
	}

	void* svc(void* x){
		
		void* res = iterative_stage_emitter_function(x);

		if(res!=NULL){

			ff_send_out(res); // to use static scheduler it must be ff_send_out otherewise it does not work
    		return GO_ON;
		
		}else return NULL;
	}

	void svc_end(){}
        
};


 /*collector*/
class collector_cls: public ff_node { 

public:
	collector_cls(){}

	int svc_init(){
	
		return 0;
	}

	void* svc(void* x){
		
		return iterative_stage_collector_function(x);
	}

	void svc_end(){}
        
};


/*components*/
class first_stage: public ff_node { 
public:
	first_stage(){}

	int svc_init(){
	
		return 0;
	}

	void* svc(void* x){
		
		return first_stage_worker_function(x);
	}

	void svc_end(){}
        
};

/*components*/
class last_stage: public ff_node { 
public:
	last_stage(){}

	int svc_init(){
	
		return 0;
	}

	void* svc(void* x){
		
		return last_stage_function(x);
	}

	void svc_end(){}
        
};

#if defined(FASTFLOW_OPENCL)
/*ocl_component*/
class ocl_component_cls: public ff_oclNode { 
private:
protected:
public:
	ocl_component_cls(){}

	int svc_init(){
		return 0;
	}

	void* svc(void* x){
		return execute_kernel(x, get_oclParameter());
	}

	void svc_end(){}
        
};

class openCLParameters : public ff_oclParam {

private: 
	std::string kernelPath;
public:
	openCLParameters(std::string krp):kernelPath(krp){}

	oclParameter*  set_oclParameter(cl_device_id dId)
	{
    	return set_oclParameters(dId, kernelPath);
    }	
};
#endif


int main(int argc, char * argv[])
{

     if (argc<2) 
    {
        std::cerr << "Insert path to GPU kernel " << argv[0] << "\n";
        return -1;
    }
    
    std::string kernel_path(argv[1]);
    ff_pipeline* root=new ff_pipeline();
    ff_node* f_stage = new first_stage();
    ff_node* l_stage= new last_stage();
    ff_node* f_emitter = new emitter_cls();
    ff_node* f_collector= new collector_cls();
   
    ff_farm<adaptive_loadbalancer, ff_gatherer> * s_farm = 
    new ff_farm<adaptive_loadbalancer, ff_gatherer>(false, 50, 50+2, false, 2, true);
  
    // preparing OpenCL
    openCLParameters* openCLObject=new openCLParameters(kernel_path); 
    root->prepare_openCL(openCLObject);

    s_farm->add_emitter(f_emitter);
    s_farm->add_collector(f_collector);
    std::vector<ff_node *> w;
   
    for(int i=0;i<2;++i) w.push_back(new ocl_component_cls());
  	s_farm->add_workers(w);
   
    root->add_stage(f_stage);
    root->add_stage(s_farm);
    root->add_stage(l_stage);

    root->run_and_wait_end();

    #if defined(TRACE_FASTFLOW)
    root->ffStats(std::cerr);
    #endif

    return 0;
	
}
