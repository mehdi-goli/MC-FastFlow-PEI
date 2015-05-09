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
 Mehdi Goli: m.goli@rgu.ac.uk*/
 /*
 This is the sobel filter application that does not use PEI for performance enhancement
 */
#include "examples/Image_processing/Funcs/functions.cpp"
#include <ff/PEI/DSRIManagerNode.hpp>
#if defined(FASTFLOW_OPENCL)
	#include <ff/oclnode.hpp>
#else
	#include <ff/node.hpp>
#endif
#include <sstream>      // std::ostringstream
#include <map>
using namespace ff;


/*components*/
class first_stage: public ff_node { 
public:
	first_stage(){}

	int svc_init(){
	
		return 0;
	}

	void* svc(void* x){
		
		return read_benchmarck(x);
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
		
		return write_benchmarck(x);
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
		return execute_kernel_sobel(x, get_oclParameter());
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
    	return set_oclParameter_sobel(dId, kernelPath);
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
    ff_node* s_stage = new ocl_component_cls();
    ff_node* l_stage= new last_stage();
   
    openCLParameters* openCLObject=new openCLParameters(kernel_path);
    root->prepare_openCL(openCLObject);
   
    root->add_stage(f_stage);
    root->add_stage(s_stage);
    root->add_stage(l_stage);

    root->run_and_wait_end();
    #if defined(TRACE_FASTFLOW)
    root->ffStats(std::cerr);
    #endif
    return 0;
	
}