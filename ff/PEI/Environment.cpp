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

#include <ff/PEI/Environment.hpp>
Environment* Environment::m_environment=NULL;

   
Environment::Environment()
{
	node_info = get_nodeInfo();
	int cpu_size=Environment::node_info->numCPU;
	int gpu_size=Environment::node_info->DevicesInfo.size();
	//std::cout<<"cpu_size: "<<cpu_size<<std::endl;
	//std::cout<<"gpu_size: "<<gpu_size<<std::endl;
	Environment::cpuMappingArray = new cpuMapInfo*[cpu_size];
	for(int i=0; i<cpu_size; ++i)
	{
	  Environment::cpuMappingArray[i]= new cpuMapInfo;
	  //std::cout<<"added_CPUNode: "<<i<<std::endl;
	}

	Environment::gpuMappingArray = new gpuMapInfo*[gpu_size];
	for(int i=0; i<gpu_size; ++i)
	{
	  Environment::gpuMappingArray[i]= new gpuMapInfo;
	  std::cout<<"*********************THis is the : "<<i<<"DEviceType:" <<Environment::node_info->DevicesInfo.at(i)->deviceType<<std::endl;
	}
	//std::cout<< "this is total number of GPU"<< gpu_size<<std::endl;
	//Environment::updateActualCPULoad();
	CPU_LIMIT=1;
	GPU_LIMIT=1;
	std::srand(std::time(0));
	//srand(time(NULL));
};

/*bool Environment::is_registered(std::string processId)
{   bool res;
	std::map<std::string, app_info*>::iterator iter =application_registery.find(processId);
	( iter!=application_registery.end() ) ? (res= true):(res= false);
	return res;
};

void Environment::register_application(std::string processId)
{
	std::map<std::string, app_info*>::iterator iter =application_registery.find(processId);
	if(iter==application_registery.end())
	{
		app_info *ap_inf = new app_info();
		ap_inf->status = NON;
		ap_inf->priority=0; 
		application_registery[processId]=ap_inf;
	}
	else
	{
		std::cout<< "it is already registered"<<std::endl;
	}
};
void Environment::calculate_age(vector<string> app_names)
{
	for(std::map<std::string, app_info*>::iterator iter=application_registery.begin(); iter!=application_registery.end(); ++iter )
	{
		((app_info*)iter->second)->priority=((app_info*)iter->second)->priority +1;
		
	}
}*/

Environment* Environment::get_instance()
{
    if(!(m_environment))
    {
     	std::cout<<"____ComeHere once__________________________________"<<std::endl;
      	Environment::m_environment= new Environment();
    }
    return Environment::m_environment;
};



int   Environment::get_cpu_limit()
{
	return CPU_LIMIT;
}
void  Environment::set_cpu_limit(int cl)
{
	CPU_LIMIT=cl;
}
int   Environment::get_gpu_limit()
{
 return GPU_LIMIT;
}
void  Environment::set_gpu_limit(int gl)
{
	GPU_LIMIT=gl;
}



void  Environment::activateGPUComponent(int id , std::string applicationName)
{
	//int id=Environment::getComponentGPUDevice(applicationName);
	Environment::get_instance()->gpuMappingArray[id]->inActiveallocatedComponents.erase(applicationName);
	Environment::get_instance()->gpuMappingArray[id]->allocatedComponents.insert(applicationName);
}
void Environment::inActiveGPUId(int id, std::string applicationName)
{
	(Environment::gpuMappingArray[id])->allocatedComponents.erase(applicationName);
	(Environment::gpuMappingArray[id])->inActiveallocatedComponents.insert(applicationName);
}	


bool Environment::inActiveallocateGPUId(int id, std::string applicationName)
{   
	std::string myAppname = applicationName;
	int pIdPosition= applicationName.find("/");
		std::string processId = applicationName.substr(0,pIdPosition);
	//	std::cout<< "this is the process ID:" <<processId<<std::endl;
	if(id!=-1)
	{
		(Environment::gpuMappingArray[id])->inActiveallocatedComponents.insert(myAppname);
	//	std::cout<< "This Id:"<< id <<"has been inserted for: " <<myAppname<<std::endl;
		std::map<string, int>::iterator it = (Environment::gpuMappingArray[id])->allocatedapplications.find(processId);
		if(it!=(Environment::gpuMappingArray[id])->allocatedapplications.end())
		{
			
          // std::cout<< "This Id:"<< id <<"has been inserted for: " <<myAppname<<std::endl;
			//std::cout<< "First:" <<it->first << "\t"<< "second: " << it->second <<std::endl;
	  		//std::cout<<"beffor adding "<<it->second<<std::endl;
	  		it->second= (it->second +1);
	 		// std::cout<<"after adding "<<it->second<<std::endl;
		}
		else
		{
	  		(Environment::gpuMappingArray[id])->allocatedapplications[processId]=1;
		}
		
		Environment::gpuMappingArray[id]->total_app = (Environment::gpuMappingArray[id]->total_app +1);
	 	// std::cout<< "error: cannot detect any device to allocate the opencl node"<< std::endl;
	  	
	  	return true;
	}
	else
		return false;
}

void Environment::inactivedeallocateGPUComponent(std::string  applicationName)
{
  	std::string myName = applicationName;
  	int pIdPosition= myName.find("/");
		std::string processId = myName.substr(0,pIdPosition);
	//	std::cout<< "this is the process ID:" <<processId<<std::endl;

    for(int i=0; i< Environment::get_instance()->node_info->DevicesInfo.size(); i++)
    {
      	std::map<string, int>::iterator it = (Environment::get_instance()->gpuMappingArray[i])->allocatedapplications.find(processId);
      	if(it!=(Environment::get_instance()->gpuMappingArray[i])->allocatedapplications.end())
      	{
      		std::cout<< "I found my Id here" << i << std::endl;
      		std::set<string>::iterator cmp_iter=(((Environment::get_instance())->gpuMappingArray[i])->inActiveallocatedComponents).find(applicationName);
      		
      		if(cmp_iter!=(Environment::get_instance()->gpuMappingArray[i])->inActiveallocatedComponents.end())
      		{
      			std::cout<< "I am in this section    -----"<< "and this is the total app"<< (((Environment::get_instance())->gpuMappingArray[i])->total_app) <<std::endl;
      			
      			Environment::get_instance()->gpuMappingArray[i]->total_app=( (Environment::get_instance()->gpuMappingArray[i]->total_app) - 1); 
      			
      			it->second =((it->second)-1);
        		if(it->second==0)
        		{	
        			std::cout<<"My total application removed"<<std::endl;
        			Environment::get_instance()->gpuMappingArray[i]->allocatedapplications.erase(it);
				}
				std::cout<<"My inactive application component has been removed."<<std::endl;
				(((Environment::get_instance())->gpuMappingArray[i])->inActiveallocatedComponents).erase(cmp_iter);
				int mySize =((Environment::get_instance())->gpuMappingArray[i])->total_app;
				std::cout<< "This is the total number of allocated to GPU"<< mySize<<std::endl;
      			
      		}
      	}
    }
}

bool Environment::allocateGPUId(int id, std::string applicationName)
{   
	std::string myAppname = applicationName;
	int pIdPosition= applicationName.find("/");
		std::string processId = applicationName.substr(0,pIdPosition);
		//std::cout<< "this is the process ID:" <<processId<<std::endl;
	if(id!=-1)
	{
		(Environment::gpuMappingArray[id])->allocatedComponents.insert(myAppname);
		//std::cout<< "This Id:"<< id <<"has been inserted for: " <<myAppname<<std::endl;
		std::map<string, int>::iterator it = (Environment::gpuMappingArray[id])->allocatedapplications.find(processId);
		if(it!=(Environment::gpuMappingArray[id])->allocatedapplications.end())
		{
			//std::cout<< "First:" <<it->first << "\t"<< "second: " << it->second <<std::endl;
	  		//std::cout<<"beffor adding "<<it->second<<std::endl;
	  		it->second= (it->second +1);
	 		// std::cout<<"after adding "<<it->second<<std::endl;
		}
		else
		{
	  		(Environment::gpuMappingArray[id])->allocatedapplications[processId]=1;
		}
		
		Environment::gpuMappingArray[id]->total_app = (Environment::gpuMappingArray[id]->total_app +1);
	 	// std::cout<< "error: cannot detect any device to allocate the opencl node"<< std::endl;
	  	
	  	return true;
	}
	else
		return false;
}
bool Environment::allocateCPUId(int id, std::string applicationName, double serviceTime, int prev_id)
{
	//int id =Environment::findBestCPUCore(std::string applicationName, double serviceTime, int prev_id);
	int pIdPosition= applicationName.find("/");
	std::string processId = applicationName.substr(0,pIdPosition);
	//std::cout<< "this is the process ID:" <<processId<<std::endl;
	if(id!=-1)
	{
		std::map<string, int>::iterator it = ((Environment::cpuMappingArray[id])->allocatedapplications).find(processId);
	    
	    if(it!=(Environment::cpuMappingArray[id])->allocatedapplications.end())
	    {
	    	 ++((Environment::cpuMappingArray[id])->allocatedapplications[processId]);
	    }
	    else
	    {
	    	(Environment::cpuMappingArray[id])->allocatedapplications[processId]=1;
	    }

	    Environment::cpuMappingArray[id]->componentsServiceTime[applicationName]=serviceTime;
	    Environment::cpuMappingArray[id]->total_time= Environment::cpuMappingArray[id]->total_time + serviceTime;
	    
	    return true;
	}
	else 
		return false;

}


/*int Environment::UpdateCPUMappingArray(std::string applicationName, double serviceTime)
{    int pIdPosition= applicationName.find("/");
	 std::string processId = applicationName.substr(0,pIdPosition);
	 std::cout<< "this is the process ID:" <<processId<<std::endl;
 
//	if(Environment::is_registered(processId))
//	{
		   for(int i=0; i< (Environment::node_info->numCPU); ++i)
		   {
		   		std::map<string, int>::iterator it = ((Environment::cpuMappingArray[i])->allocatedapplications).find(applicationName);
		   		if(it!=(Environment::cpuMappingArray[i])->allocatedapplications.end())
	    		{
	      			(Environment::cpuMappingArray[i])->total_time=Environment::cpuMappingArray[i]->total_time - it->second;
	      			it->second =serviceTime;
	    		}
		   }
	//}
	

	return 0;


}*/
/*bool Environment::swapGPUComponent(std::string firstAppName, std::string firstComponentName, std::string secondAppName, std::string secondComponentName)

{   std::string fitstApp = firstAppName + firstComponentName;
	std::string secondApp = secondAppName + secondComponentName;	
	int firstCompGPU =Environment::getComponentGPUDevice(fitstApp);
	int secondCompGPU =Environment::getComponentGPUDevice(secondApp);
	if(firstCompGPU==secondCompGPU) return false;
	Environment::deallocateGPUComponent(firstComponentName);
	Environment::deallocateGPUComponent(secondComponentName);
	Environment::allocateGPUId(secondCompGPU, fitstApp);
	//Environment::allocateGPUId(secondCompGPU, firstAppName);
	Environment::allocateGPUId(firstCompGPU, secondApp);
	return true;
};

bool Environment::swapCPUComponent(std::string firstAppName, std::string firstComponentName, std::string secondAppName, std::string secondComponentName)
{
	std::string firstApp = firstAppName+ "/" + firstComponentName;
	std::string secondApp = secondAppName + "/" + secondComponentName;
	int firstCompCPU =Environment::getComponentCPUCore(firstApp);
	int secondCompCPU =Environment::getComponentCPUCore(secondApp);
	if(firstCompCPU=secondCompCPU) return false;
	Environment::deallocateCPUComponent(firstComponentName);
	Environment::deallocateCPUComponent(secondComponentName);
	Environment::allocateCPUId(secondCompCPU, firstAppName, Environment::cpuMappingArray[firstCompCPU]->componentsServiceTime.at(firstApp), firstCompCPU);
	Environment::allocateCPUId(firstCompCPU, secondAppName,Environment::cpuMappingArray[secondCompCPU]->componentsServiceTime.at(firstApp), secondCompCPU);
	return true;	
};*/
void Environment::releaseResources(std::string processId){
	std::cout<< "+++++++++++++++++++ release callled++++++++++++++++++++++++++++++"<<std::endl;
	deallocateCPUId(processId);
	deallocateGPUId(processId);	
}
void Environment::deallocateCPUComponent(std::string  applicationName)
{	int pIdPosition= applicationName.find("/");
		std::string processId = applicationName.substr(0,pIdPosition);
	//	std::cout<< "this is the process ID:" <<processId<<std::endl;
    for(int i=0; i< Environment::node_info->numCPU; ++i)
    {
      std::map<string, int>::iterator it = ((Environment::cpuMappingArray[i])->allocatedapplications).find(processId);
      if(it!=(Environment::cpuMappingArray[i])->allocatedapplications.end())
      {
      	std::map<string, double>::iterator cmp_iter = (Environment::cpuMappingArray[i])->componentsServiceTime.find(applicationName);
      	if(cmp_iter!=(Environment::cpuMappingArray[i])->componentsServiceTime.end())
      	{
      		Environment::cpuMappingArray[i]->total_time= (Environment::cpuMappingArray[i]->total_time) - (cmp_iter->second); 
        	it->second = it->second-1;
        	if(it->second==0)
        	{	
        		Environment::cpuMappingArray[i]->allocatedapplications.erase(it);
			}

        	Environment::cpuMappingArray[i]->componentsServiceTime.erase(cmp_iter);
      	}
        
      }
    }
};

void Environment::deallocateCPUId(std::string  processId)
{	
    for(int i=0; i< Environment::node_info->numCPU; ++i)
    {
      std::map<string, int>::iterator it = ((Environment::cpuMappingArray[i])->allocatedapplications).find(processId);
      if(it!=(Environment::cpuMappingArray[i])->allocatedapplications.end())
      {
      	Environment::cpuMappingArray[i]->allocatedapplications.erase(it);
      	for(std::map<string, double>::iterator cmp_iter=(Environment::cpuMappingArray[i])->componentsServiceTime.begin();
      		cmp_iter!=(Environment::cpuMappingArray[i])->componentsServiceTime.end(); ++cmp_iter)
      	{
      		std::string applicationName=cmp_iter->first;
      		int pIdPosition= applicationName.find("/");
			std::string myProcessId = applicationName.substr(0,pIdPosition);
	//		std::cout<< "this is the process ID:" <<myProcessId<<std::endl;	
			if(strcasecmp(myProcessId.data(),processId.data())==0)
			{
				Environment::cpuMappingArray[i]->total_time= (Environment::cpuMappingArray[i]->total_time) - (cmp_iter->second); 
				Environment::cpuMappingArray[i]->componentsServiceTime.erase(cmp_iter);
			}
      	}
      }
    }
};



void Environment::deallocateGPUComponent(std::string  applicationName)
{
  	std::string myName = applicationName;
  	int pIdPosition= myName.find("/");
		std::string processId = myName.substr(0,pIdPosition);
	//	std::cout<< "this is the process ID:" <<processId<<std::endl;

    for(int i=0; i< Environment::get_instance()->node_info->DevicesInfo.size(); i++)
    {
      	std::map<string, int>::iterator it = (Environment::get_instance()->gpuMappingArray[i])->allocatedapplications.find(processId);
      	if(it!=(Environment::get_instance()->gpuMappingArray[i])->allocatedapplications.end())
      	{
      //		std::cout<< "I found my Id here" << i << std::endl;
      		std::set<string>::iterator cmp_iter=(((Environment::get_instance())->gpuMappingArray[i])->allocatedComponents).find(applicationName);
      		
      		if(cmp_iter!=(Environment::get_instance()->gpuMappingArray[i])->allocatedComponents.end())
      		{
      	//		std::cout<< "I am in this section    -----"<< "and this is the total app"<< (((Environment::get_instance())->gpuMappingArray[i])->total_app) <<std::endl;
      			
      			Environment::get_instance()->gpuMappingArray[i]->total_app=( (Environment::get_instance()->gpuMappingArray[i]->total_app) - 1); 
      			
      			it->second =((it->second)-1);
        		if(it->second==0)
        		{	
        //			std::cout<<"My application removed"<<std::endl;
        			Environment::get_instance()->gpuMappingArray[i]->allocatedapplications.erase(it);
				}
		//		std::cout<<"My application component has been removed."<<std::endl;
				(((Environment::get_instance())->gpuMappingArray[i])->allocatedComponents).erase(cmp_iter);
				int mySize =((Environment::get_instance())->gpuMappingArray[i])->total_app;
		//		std::cout<< "This is the total number of allocated to GPU"<< mySize<<std::endl;
      			
      		}


      		// following part is added to deallocate inactive component no need here
      		/* cmp_iter=(((Environment::get_instance())->gpuMappingArray[i])->inActiveallocatedComponents).find(applicationName);
      		
      		if(cmp_iter!=(Environment::get_instance()->gpuMappingArray[i])->inActiveallocatedComponents.end())
      		{
      	//		std::cout<< "I am in this section    -----"<< "and this is the total app"<< (((Environment::get_instance())->gpuMappingArray[i])->total_app) <<std::endl;
      			
      			Environment::get_instance()->gpuMappingArray[i]->total_app=( (Environment::get_instance()->gpuMappingArray[i]->total_app) - 1); 
      			
      			it->second =((it->second)-1);
        		if(it->second==0)
        		{	
        //			std::cout<<"My total application removed"<<std::endl;
        			Environment::get_instance()->gpuMappingArray[i]->allocatedapplications.erase(it);
				}
		//		std::cout<<"My inactive application component has been removed."<<std::endl;
				(((Environment::get_instance())->gpuMappingArray[i])->inActiveallocatedComponents).erase(cmp_iter);
				//int mySize =((Environment::get_instance())->gpuMappingArray[i])->total_app;
		//		std::cout<< "This is the total number of allocated to GPU"<< mySize<<std::endl;
      			
      		}*/
      	}
    }
}


/*int Environment::updateActualCPULoad()
{
	FILE *fp;
  	unsigned long long int fields[10], total_tick[MAX_CPU], total_tick_old[MAX_CPU], idle[MAX_CPU], idle_old[MAX_CPU], del_total_tick[MAX_CPU], del_idle[MAX_CPU];
  	int update_cycle = 0, i, cpus = 0, count;
  	double percent_usage;

  	fp = fopen ("/proc/stat", "r");
  	if (fp == NULL)
  	{
    	perror ("Error");
  	}
 
 	while (read_fields (fp, fields) != -1)
  	{
    	for (i=0, total_tick[cpus] = 0; i<10; i++)
    	{ 
    		total_tick[cpus] += fields[i]; 
    	}
    	
    	idle[cpus] = fields[3]; // idle ticks index 
    	cpus++;
  	}
 
    fseek (fp, 0, SEEK_SET);
    fflush (fp);
    printf ("[Update cycle %d]\n", update_cycle); 
    for (count = 0; count < cpus; count++)
    {
    	if (count !=0)
    	{
	  		total_tick_old[count] =cpuMappingArray[count-1]->total_tick;
	  		cpuMappingArray[count-1]->total_tick=total_tick[count];
	  		idle_old[count] = cpuMappingArray[count-1]->idle_tick;
	 		cpuMappingArray[count-1]->idle_tick=idle[count];
	 	}
	  	if (count != 0)
	  	{	
	  		del_total_tick[count] = total_tick[count] - total_tick_old[count];
	  		del_idle[count] = idle[count] - idle_old[count];

	  		percent_usage = ((del_total_tick[count] - del_idle[count]) / (double) del_total_tick[count]) * 100;
	  	 	
	  		printf ("\tCPU%d Usage: %3.2lf%%\n", count - 1, percent_usage); 
	  		cpuMappingArray[count-1]->load_percentage=percent_usage;
		}
    }
    update_cycle++;
    printf ("\n"); 
  // Ctrl + C quit, therefore this will not be reached. We rely on the kernel to close this file 
  fclose (fp);
 return 0;
 
}*/
int  Environment::getComponentGPUDevice(std::string applicationName)
{

	std::string myAppname= applicationName;
	int pIdPosition= applicationName.find("/");
	std::string processId = applicationName.substr(0,pIdPosition);
//	std::cout<< "this is the process ID:" <<processId<<std::endl;
	//std::cout<< "this is My app name:" <<myAppname<<std::endl;

    for(int i=0; i< Environment::node_info->DevicesInfo.size(); i++)
    {
      	std::map<string, int>::iterator it = (Environment::gpuMappingArray[i])->allocatedapplications.find(processId);
      	if(it!=(Environment::gpuMappingArray[i])->allocatedapplications.end())
      	{
  //    		std::cout<< "this is My found GPU:" <<i<<std::endl;
      		std::set<string>::iterator cmp_iter=(Environment::gpuMappingArray[i])->allocatedComponents.find(myAppname);
      		if(cmp_iter!=(Environment::gpuMappingArray[i])->allocatedComponents.end()) return i;
      		//for inactive
      		cmp_iter=(Environment::gpuMappingArray[i])->inActiveallocatedComponents.find(myAppname);
      		if(cmp_iter!=(Environment::gpuMappingArray[i])->inActiveallocatedComponents.end()) return i;
		}
	}
	return -1;

}
bool Environment::updateCPUComponetServiceTime(std::string applicationName, double serviceTime ) 
{
	int cpuId=getComponentCPUCore(applicationName);
	if(cpuId!=-1)
	{
	std::map<string, double>::iterator cmp_iter = (Environment::cpuMappingArray[cpuId])->componentsServiceTime.find(applicationName);
	Environment::cpuMappingArray[cpuId]->total_time= (Environment::cpuMappingArray[cpuId]->total_time) - (cmp_iter->second); 
	cmp_iter->second=serviceTime;
	Environment::cpuMappingArray[cpuId]->total_time= (Environment::cpuMappingArray[cpuId]->total_time) + (cmp_iter->second); 
	return true;
	}//else std::cout<< "+++++++++++++++++++++++++++++++++++++I didnot find My CPU ID++++++++++++++++++++++++++++++++++++++++++++++++++++"<< std::endl;

}
/*bool Environment::realocateCPUComponent(std::string applicationName, int new_id) 
{
	int cpuId=getComponentCPUCore(applicationName);
	std::map<string, double>::iterator cmp_iter = (Environment::cpuMappingArray[cpuId])->componentsServiceTime.find(applicationName);
	double serviceTime=cmp_iter->second;
	deallocateCPUComponent(applicationName);
	allocateCPUId(new_id,applicationName,serviceTime , cpuId);

	return true;

}*/
int Environment::getComponentCPUCore(std::string applicationName ) 
{

	int pIdPosition= applicationName.find("/");
	std::string processId = applicationName.substr(0,pIdPosition);
	//std::cout<< "this is the process ID:" <<processId<<std::endl;
    for(int i=0; i< Environment::node_info->numCPU; ++i)
    {
      	std::map<string, int>::iterator it = ((Environment::cpuMappingArray[i])->allocatedapplications).find(processId);
      	if(it!=(Environment::cpuMappingArray[i])->allocatedapplications.end())
      	{
      		std::map<string, double>::iterator cmp_iter = (Environment::cpuMappingArray[i])->componentsServiceTime.find(applicationName);
      		if(cmp_iter!=(Environment::cpuMappingArray[i])->componentsServiceTime.end()) return i;
		}
	}
	return -1;	
}

void Environment::deallocateGPUId(std::string  processId)
  {

    for(int i=0; i< Environment::node_info->DevicesInfo.size(); i++)
    {
      std::map<string, int>::iterator it = (Environment::gpuMappingArray[i])->allocatedapplications.find(processId);
      if(it!=(Environment::gpuMappingArray[i])->allocatedapplications.end())
      {

        Environment::gpuMappingArray[i]->total_app=( (Environment::gpuMappingArray[i]->total_app) - (it->second)); 
        (Environment::gpuMappingArray[i])->allocatedapplications.erase(it);

        for(std::set<string>::iterator cmp_iter=(Environment::gpuMappingArray[i])->allocatedComponents.begin();
      		cmp_iter!=(Environment::gpuMappingArray[i])->allocatedComponents.end(); ++cmp_iter)
      	{
      		std::string applicationName=(*cmp_iter);
      		int pIdPosition= applicationName.find("/");
			std::string myProcessId = applicationName.substr(0,pIdPosition);
	//		std::cout<< "this is the process ID:" <<myProcessId<<std::endl;	
			if(strcasecmp(myProcessId.data(),processId.data())==0)
			{
				Environment::gpuMappingArray[i]->allocatedComponents.erase(cmp_iter);
			}
      	}



      	for(std::set<string>::iterator cmp_iter=(Environment::gpuMappingArray[i])->inActiveallocatedComponents.begin();
      		cmp_iter!=(Environment::gpuMappingArray[i])->inActiveallocatedComponents.end(); ++cmp_iter)
      	{
      		std::string applicationName=(*cmp_iter);
      		int pIdPosition= applicationName.find("/");
			std::string myProcessId = applicationName.substr(0,pIdPosition);
	//		std::cout<< "this is the process ID:" <<myProcessId<<std::endl;	
			if(strcasecmp(myProcessId.data(),processId.data())==0)
			{
				Environment::gpuMappingArray[i]->inActiveallocatedComponents.erase(cmp_iter);
			}
      	}



      }
    }
  }

  std::string Environment::getComponent(std::string appId, std::string devType)
  	{
  		std::string node_address;
  		for(int i=0; i<(Environment::get_instance()->node_info->DevicesInfo).size();++i)
  		{	
  			std::string dev_type=Environment::get_instance()->node_info->DevicesInfo[i]->deviceType;
  			if((strcasecmp(dev_type.c_str(), devType.c_str())==0))
  			{
  				for(std::set<std::string>::iterator it=(Environment::get_instance()->gpuMappingArray[i]->allocatedComponents).begin();
					it!=(Environment::get_instance()->gpuMappingArray[i]->allocatedComponents).end(); ++it )
				{
	//				std::cout<< "this is the node name ********************"<<(*it)<< std::endl;
					node_address=(*it);
					int victimIdPosition= (*it).find("/");
					std::string victimId = (*it).substr(0,victimIdPosition);
					if ((strcasecmp((appId).c_str(), victimId.c_str())==0))
					{
						return node_address;
					}
				}
			}

		}
		node_address.clear();
		return node_address;
  	}
  	int Environment::get_rand(int seed)
  	{
  		return ((std::rand())%seed);
  	}

  	std::string Environment::getComponent_rand(std::string appId, std::string devType)
  	{
  		std::string node_address;
  		std::vector<std::string> victim_nodes;
  		for(int i=0; i<(Environment::get_instance()->node_info->DevicesInfo).size();++i)
  		{	
  			std::string dev_type=Environment::get_instance()->node_info->DevicesInfo[i]->deviceType;

  			if((strcasecmp(dev_type.c_str(), devType.c_str())==0))
  			{
	  			for(std::set<std::string>::iterator it=(Environment::get_instance()->gpuMappingArray[i]->allocatedComponents).begin();
					it!=(Environment::get_instance()->gpuMappingArray[i]->allocatedComponents).end(); ++it )
				{
					node_address=(*it);
					int victimIdPosition= (*it).find("/");
					std::string victimId = (*it).substr(0,victimIdPosition);
					if ((strcasecmp((appId).c_str(), victimId.c_str())==0))
					{
						victim_nodes.push_back(node_address);
					}
				}
			}

		}
		
		node_address.clear();
		if (victim_nodes.size()==0) 
			return node_address;
		else 
		{
			int i = Environment::get_instance()->get_rand(victim_nodes.size());
			return victim_nodes.at(i);
		}
  	}


  	std::string Environment::getInactiveComponent(std::string appId, std::string devType)
  	{
  		for(int i=0; i<(Environment::get_instance()->node_info->DevicesInfo).size();++i)
  		{	
  			std::string dev_type=Environment::get_instance()->node_info->DevicesInfo[i]->deviceType;
			for(std::set<std::string>::iterator it=(Environment::get_instance()->gpuMappingArray[i]->inActiveallocatedComponents).begin();
				it!=(Environment::get_instance()->gpuMappingArray[i]->inActiveallocatedComponents).end(); ++it )
			{
					std::string node_address=(*it);
					int victimIdPosition= (*it).find("/");
					std::string victimId = (*it).substr(0,victimIdPosition);
					if ((strcasecmp((appId).c_str(), victimId.c_str())==0)&& (strcasecmp(dev_type.c_str(), devType.c_str())==0))
					{
						return node_address;
					}
			}
		}
		return NULL;
  	}




