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

#include <ff/PEI/Mapping_Heuristic.hpp>

mappingHeuristics::mappingHeuristics(){};
template<typename T>
bool  mappingHeuristics::editActuator(JSONNode& edittedNode, std::string nodeName , std::string edited_name , T edited_value)
{
    JSONNode::iterator jnIt= edittedNode.find_nocase("Node_Address");
   // if(jnIt!=edittedNode.end())
    //{
   // 	std::cout<< "**************************This is node_address**************************************"<<jnIt->as_string()<< std::endl;
	//}

    if(jnIt!=edittedNode.end() && strcasecmp((jnIt->as_string()).c_str(), nodeName.c_str()) == 0)
    {
        edittedNode.at_nocase(edited_name)=edited_value;
       // std::cout<< "**************your value has been eddited here for :"<< edited_name<< " to : " << edited_value<<std::endl;
        return true;
    }
    else
    {
        JSONNode::iterator i = edittedNode.begin();
        while (i != edittedNode.end())
        {
            if (i -> type() == JSON_ARRAY || i -> type() == JSON_NODE)
                if(editActuator<T>(*i, nodeName, edited_name, edited_value)) 
                    return true;
            ++i;
        }
        return false;
    }
}


void mappingHeuristics::get_object(JSONNode& jn, std::string nodeName , JSONNode& res){


	   
	   JSONNode::iterator jnIt= jn.find_nocase("Node_Address");
	   std::cout<< "this is parrent name:" << nodeName<< std::endl;
	   std::cout<< "this is parrent object:" << jn.write_formatted()<< std::endl;


    if(jnIt!=jn.end() && strcasecmp((jnIt->as_string()).c_str(), nodeName.c_str()) == 0)
    {
       
       res =jn;
    	//JSONNode::const_iterator it =jn.find_nocase("Parallel");
        //if(it==jn.end())
       // {
       // 	it =jn.find_nocase("MISD");
       // }
	   // JSONNode workersSensors = it->as_array();
       // JSONNode workersSensorsObject = workersSensors[0].as_node();
    //	return jn;
	    //return workersSensorsObject.size();
        
    }
    else
    {
        JSONNode::iterator i = jn.begin();
        while (i != jn.end())
        {
            if (i -> type() == JSON_ARRAY || i -> type() == JSON_NODE)
            {
            	get_object(*i, nodeName, res);
                //if(val>0) return val;
            }
            ++i;
        }
    //return (JSON_NODE::JSON_NULL);
    }

}

bool mappingHeuristics::try_unmasking(std::string applicationName)
{	
	//std::string myName =applicationName;
 	int dev_id=Environment::get_instance()->getComponentGPUDevice(applicationName);
 	std::cout<<"!!!!!!!!!!!!!THIS IS DEVID: "<<dev_id<<std::endl;
	int num= (Environment::get_instance()->gpuMappingArray[dev_id]->total_app);
	if(num<get_max_ocl_task_per_device(dev_id))
	{
		Environment::get_instance()->activateGPUComponent(dev_id,applicationName);
		unMaskComponentDevice(applicationName);
		return true;
	} 
	return false;
}
void mappingHeuristics::deregister_masked(std::string processId)
{
	
	//map<string,farmStructure*>::iterator it = farmMask.begin();
	

	//farmMask.erase(key);
	std::vector<std::string> toBeDeletedkeys;
	for(map<string,farmStructure*>::iterator it = farmMask.begin(); it!=farmMask.end(); ++it)
	{
		std::string key=it->first;
		int pIdPosition= (it->first).find_first_of("/");
    	std::string app_id = (it->first).substr(0,pIdPosition);
    	//std::cout<<"£££££££££££££££££££££££this is the key££££££££"<<app_id<<std::endl;
    	if(strcasecmp(app_id.c_str(), processId.c_str())==0)
    	{
			map<string,farmStructure*>::iterator it = farmMask.find(key);

			farmStructure* farmStruct=(it->second);
			//std::cout<< "()__))))))))))))))______I am deregistering ____))_)_))_______"<<processId<<std::endl;
			for (std::set<std::string>::iterator s_it=farmStruct->cmpAddress.begin(); s_it!=farmStruct->cmpAddress.end(); ++s_it)
			{
				//std::cout<< "()__))))))))))))£££ ths is my value"<<(*s_it)<<std::endl;
				std::string applicationName = app_id + "/" + (*s_it);
				unMaskComponentDevice(applicationName);
			}

			toBeDeletedkeys.push_back(key);
			//farmMask.erase(it);
			//it = farmMask.find(processId);
		}

	}
	for(int i=0; i<toBeDeletedkeys.size(); ++i)
	{
		farmMask.erase(toBeDeletedkeys.at(i));
		//std::cout<< "()__))))))))))))))______is deregistered ____))_)_))_______"<<toBeDeletedkeys.at(i)<<std::endl;
	}
}

int mappingHeuristics::try_masking(std::string applicationName)
{
	int cl_id=-1;
	int pIdPosition= applicationName.find("/");
	std::string processId = applicationName.substr(0,pIdPosition);
	std::string cmpNode_address=applicationName.substr(pIdPosition+1);
	
		unsigned found = cmpNode_address.find_last_of("/");
  		std::string url_path=cmpNode_address.substr(0,found);
        std::string cmp_id= cmpNode_address.substr(found+1);
		
		found= url_path.find_last_of("/");
		std::string parent_path=url_path.substr(0,found);
		std::string cmp_name = url_path.substr(found+1);

		found= parent_path.find_last_of("/");
		std::string parrent_url=parent_path.substr(0,found);
		std::string parent_id = parent_path.substr(found+1);

		found= parrent_url.find_last_of("/");
		std::string gparrent_url=parrent_url.substr(0,found);
		std::string parent_name = parrent_url.substr(found+1);
		
		JSONNode jn_obj = libjson::parse(registered_apps[processId]->sensor);
		JSONNode jn_bject; 
		mappingHeuristics::get_object(jn_obj, "/" + parent_path, jn_bject);
		JSONNode::const_iterator it=jn_bject.find_nocase("Component_Type");
		JSONNode::const_iterator it_misd=jn_bject.find_nocase("MISD");

		if((it!=jn_bject.end() && ((it->as_string()).find("farm")!=std::string::npos))&&(it_misd==jn_bject.end()))
		{
			std::cout<< "+++++++++++++++++++++++++++++++++____I am Farm_____++++++++++++++++++++++++++++++++++++++++++"<<std::endl;
			it = jn_bject.find_nocase("1ToNCom");
			std::string lbType = (it->as_node()).at_nocase("Ch_Policy").as_string();

		//std::cout<< "This is my Parent Name"<< parent_name<<std::endl;

			if (lbType.find("adaptive_loadbalancer") != std::string::npos) {
	    		//std::cout << "found!*****************" << '\n';
	    		std::string key = processId + "/" + parent_name+ "/" +parent_id ;
	    		std::map<string,farmStructure*>::iterator it= farmMask.find(key);
	    		if(it!=farmMask.end())
	    		{
	    			farmStructure* fs=it->second;
	    			std::pair<std::set<std::string>::iterator,bool> ret;
	    			ret= fs->cmpAddress.insert(cmpNode_address);
	  				if (ret.second==true) 
	  				{
	  			//		std::cout<< "APPName: " <<applicationName<< "is masked here"<<std::endl;
	  					++(fs->numSuspendedCount);
	  					cl_id=maskComponentDevice(applicationName);
	  				}
	  				//std::cout<< "&&&&&&&&&&&&&& This is farm workers : "<< fs->totalNumWorkers<<std::endl;
	  				//std::cout<< "&&&&&&&&&&&&&& This is suspended workers : "<< fs->numSuspendedCount<<std::endl;
	  				if(fs->numSuspendedCount>=fs->totalNumWorkers) //return true;
	  				//else 
	  				{
						std::cout<< "====================we have to be suspended :=================="<< processId<<"================" <<applicationName<< "this is CL_id"<< cl_id<<std::endl;
						deregister_masked(processId);
						//return false;
						cl_id=-1;
	  				}
	    		}
	    		else
	    		{
	    			//std::cout<< "I am new and :" <<applicationName<< "is masked here"<<std::endl;
	    			JSONNode jn = libjson::parse(registered_apps[processId]->sensor);
					int nw =findWorkerNumbers(jn, "/" + parent_path);
	    			farmStructure* fs= new farmStructure(nw, parent_path);
	    			++fs->numSuspendedCount;
	    			cl_id = maskComponentDevice(applicationName);
	    			//if(prev_id==-1) Environment::get_instance()->allocateGPUId(min_val(masked_cpu, masked_gpu), applicationName);
	    			fs->cmpAddress.insert(cmpNode_address);
	    			farmMask[key]=fs;
	    			if(fs->numSuspendedCount>=fs->totalNumWorkers) //return true;
	  				//else 
	  				{
						std::cout<< "====================we have to be suspended :=================="<< processId<<"================" <<applicationName<< "this is CL_id"<< cl_id<<std::endl;
						deregister_masked(processId);
						//return false;
						cl_id=-1;
	  				}

	    			//return true;
	    		}
			}
		}
		return cl_id;
}
int mappingHeuristics::findWorkerNumbers(JSONNode& sensors, std::string nodeName)
{
    JSONNode::iterator jnIt= sensors.find_nocase("Node_Address");
   // std::cout<< "**************************This is node_name looking**************************************"<<nodeName<< std::endl;
 //   if(jnIt!=sensors.end())
   // {
    //	std::cout<< "**************************This is node_address i saw**************************************"<<jnIt->as_string()<< std::endl;
	//}

    if(jnIt!=sensors.end() && strcasecmp((jnIt->as_string()).c_str(), nodeName.c_str()) == 0)
    {
       
    	JSONNode::const_iterator it =sensors.find_nocase("Parallel");
        if(it==sensors.end())
        {
        	it =sensors.find_nocase("MISD");
        }
	    JSONNode workersSensors = it->as_array();
        JSONNode workersSensorsObject = workersSensors[0].as_node();

        //JSONNode::const_iterator it =sensors.find_nocase("Workers");
	    //JSONNode workersSensors = it->as_array();
	    return workersSensorsObject.size();
        //std::cout<< "**************your value has been eddited here for :"<< edited_name<< " to : " << edited_value<<std::endl;
        //return true;
    }
    else
    {
        JSONNode::iterator i = sensors.begin();
        while (i != sensors.end())
        {
            if (i -> type() == JSON_ARRAY || i -> type() == JSON_NODE)
            {
            	int val =findWorkerNumbers(*i, nodeName);
                if(val>0) return val;
            }
            ++i;
        }
        return -1;
    }
}


// this is the priority based policy. This policy can be changed in any other policy like
// buttleneck based policy tc.


int mappingHeuristics::get_max_ocl_task_per_device(int id_in_node_info)
{ 
    int dev_sm = Environment::get_instance()->node_info->DevicesInfo.at(id_in_node_info)->SMNumbers;
    // any other rule can be set here. although at tne momnet I must reduce the chances of CPU selection which still is high
       std::string dev_type=Environment::get_instance()->node_info->DevicesInfo[id_in_node_info]->deviceType;
    // if((strcasecmp("CL_DEVICE_TYPE_CPU", dev_type.c_str()) == 0)){dev_sm*=4;}
    // return dev_sm/2;
       if((strcasecmp("CL_DEVICE_TYPE_CPU", dev_type.c_str()) == 0)) return (Environment::get_instance()->get_cpu_limit() + masked_cpu);
       else  return (Environment::get_instance()->get_gpu_limit()+ masked_gpu);
};

 void mappingHeuristics::getParrentPath(std::string applicationName, std::string & parent_name, std::string& parent_path)
{
	unsigned found = applicationName.find_last_of("/");
	std::string url_path=applicationName.substr(0,found);
    std::string cmp_id= applicationName.substr(found+1);
	
	found= url_path.find_last_of("/");
	parent_path=url_path.substr(0,found);
	std::string cmp_name = url_path.substr(found+1);

	found= parent_path.find_last_of("/");
	std::string parrent_url=parent_path.substr(0,found);
	std::string parent_id = parent_path.substr(found+1);

	found= parrent_url.find_last_of("/");
	std::string gparrent_url=parrent_url.substr(0,found);
	parent_name = parrent_url.substr(found+1);
}
bool mappingHeuristics::SwappingYourself(std::string applicationName, std::string cl_dev_type_constraint)
{
	
	//std::cout<< "This is my Parent Name"<< parent_name<<std::endl;

	int r =Environment::get_instance()->get_rand(2);
	if(r)
	{
		std::string myName =applicationName;
		int pIdPosition= myName.find("/");
		std::string processId = myName.substr(0,pIdPosition);
		std::string my_component_name = myName.substr(pIdPosition);

		std::string cmp = Environment::get_instance()->getComponent_rand(processId,cl_dev_type_constraint);

		if(!(cmp.empty()))
		{	
				std::string myParentName;
				std::string myParentPath;
				std::string cmpParentName;
				std::string cmpParentPath;
				getParrentPath(applicationName, myParentName, myParentPath);
				getParrentPath(cmp, cmpParentName, cmpParentPath);

				if((strcasecmp(myParentPath.c_str(), cmpParentPath.c_str())==0)
					&& (myParentName.find("ff_farm") != std::string::npos)) 
				{
					std::cout<<"both nodes of a farm***************************"<<applicationName<<std::endl;
					std::cout<<"both nodes of a farm***************************"<<cmp<<std::endl;
					return false;
				}
				std::string cmpName =cmp;
				int cmpPIdPosition= cmpName.find("/");
				std::cout<<"this is cmp***************************"<<cmp<<std::endl;
				std::cout<<"this is cmpName***************************"<<cmpName<<std::endl;
				std::cout<<"this is cmpPIdPosition***************************"<<cmpPIdPosition<<std::endl;
				std::string cmp_component_name = cmpName.substr(cmpPIdPosition);
				int myId=Environment::get_instance()->getComponentGPUDevice(applicationName);
				std::cout<< "++++++++++++myId before swap"<< myId<<std::endl;
				int cmpId= Environment::get_instance()->getComponentGPUDevice(cmp);
				std::cout<< "++++++++++++cmpId before swap"<< cmpId<<std::endl;
				Environment::get_instance()->deallocateGPUComponent(cmp);
				Environment::get_instance()->deallocateGPUComponent(applicationName);
				Environment::get_instance()->allocateGPUId(myId, cmp);
				Environment::get_instance()->allocateGPUId(cmpId, applicationName);

				//Environment::get_instance()->swapGPUComponent(processId, my_component_name, processId, cmp_component_name);

				 myId=Environment::get_instance()->getComponentGPUDevice(applicationName);
				std::cout<<"this is myName***************************"<< applicationName<<std::endl;
				std::cout<<"this is my commponent***************************"<< my_component_name<<std::endl;
				std::cout<< "This is my swapped new device ++++++++++++++++++++++++++++++ " << myId<<std::endl;
				 cmpId= Environment::get_instance()->getComponentGPUDevice(cmp);
				std::cout<<"this is myName========================================"<< cmp<<std::endl;
				std::cout<<"this is my commponent===================================="<< cmp_component_name<<std::endl;
				std::cout<< "This is my cmp swapped new device ==============================" << cmpId<<std::endl;

				JSONNode jn = libjson::parse(registered_apps[processId]->actuator);
				editActuator<int>(jn, my_component_name, "Device_Number", myId);
				editActuator<int>(jn, cmp_component_name, "Device_Number", cmpId);
				registered_apps[processId]->actuator=jn.write_formatted();
				return true;

		}
	}
	return false;
}

bool  mappingHeuristics::moveToGPU(std::string applicationName)
{
 	//std::cout<< "This is appname for improve:++++++++++++++++++++++++++++++ " << applicationName<<std::endl;
 	std::string myName =applicationName;
 	int prev_id=Environment::get_instance()->getComponentGPUDevice(applicationName);
 	//if(prev_id==-1) return false;
 	std::cout<< "MyName:"<< myName <<std::endl;
 	std::cout<< "This is my prev_Id for over ocl device to improve:++++++++++++++++++++++++++++++ " << prev_id<<std::endl;
 	std::string dev_type=Environment::get_instance()->node_info->DevicesInfo.at(prev_id)->deviceType;
	if (strcasecmp(dev_type.c_str(), "CL_DEVICE_TYPE_CPU")==0)
	{
	 	//std::cout << "my Id: " << prev_id<< "is CPU"<<"and this is me: "<< applicationName<<std::endl;
	 	Environment::get_instance()->deallocateGPUComponent(applicationName);
	 	int cl_id =findBestGPUDeviceByType(applicationName, "CL_DEVICE_TYPE_GPU"); // try to move to gpu
	 	if (cl_id!=-1)
	 	{
	 		int pIdPosition= myName.find("/");
			std::string processId = myName.substr(0,pIdPosition);
	 		//std::string node_address=applicationName;
			//int victimIdPosition= applicationName.find("/");
			std::string my_component_name = myName.substr(pIdPosition);
		//	std::cout<< "This is the my component address:****************************"<< my_component_name<< std::endl;

	 		JSONNode jn = libjson::parse(registered_apps[processId]->actuator);
			editActuator<std::string>(jn, my_component_name, "Device_Number", static_cast<ostringstream*>( &(ostringstream() << cl_id) )->str());
			registered_apps[processId]->actuator=jn.write_formatted();
			Environment::get_instance()->allocateGPUId(cl_id,applicationName);
			//std::cout<< "I have been moved to GPU "<< cl_id<< std::endl;
			//std::cout<< ((registered_apps[processId])->actuator)<<std::endl;
			// I only change myself and i updated mine by myself there is no need for that
			//set<string>::iterator m_iter = modified_actuators_apps.find(processId);
			//if(m_iter==modified_actuators_apps.end())
		   // 	modified_actuators_apps.insert(processId);	
		    return true;
	 	}
	 	else
	 	{
	 		Environment::get_instance()->allocateGPUId(prev_id,applicationName);
	 		// uncomment it to activate swapping yourself
	 		std::cout << "I could not find any GPU then I came back to CPU. Try swapping with myself...."<<std::endl;
	 		if(!(SwappingYourself(applicationName,"CL_DEVICE_TYPE_GPU")))
	 		{
	 			std::cout << "I could not find any GPU then I came back to CPU"<< std::endl;
	 		}else
	 		{ 
	 			std::cout <<" *******************swapt****************8888"<<std::endl;
	 			return true;
	 		}

	 	}
	 	
	}
	else { std::cout<< "the device is already to GPU"<<std::endl;}
	return false;
}

int mappingHeuristics::maskComponentDevice(std::string applicationName,int num)

{
	int pIdPosition= applicationName.find("/");
	std::string processId = applicationName.substr(0,pIdPosition);
	int prev_id=Environment::get_instance()->getComponentGPUDevice(applicationName);
 	if(prev_id==-1) {
     	for(int i=0; i<Environment::get_instance()->node_info->DevicesInfo.size();++i)
     	{
     		std::string dev_type=Environment::get_instance()->node_info->DevicesInfo.at(i)->deviceType;
     		std::string current_type=min_val(masked_cpu,masked_gpu);
     		if(strcasecmp(current_type.c_str(),dev_type.c_str())==0)
     		{
  	 			//std::cout<< "The device allocated to:++++++++++++++++++++++++++++++ " << applicationName<<dev_type<<std::endl;
  	 			//Environment::get_instance()->allocateGPUId(i, applicationName);
  	 			prev_id=i;
  	 			Environment::get_instance()->inActiveallocateGPUId(prev_id,applicationName);	
  	 			break;
     		}
  		}
 	}else
 	{
 		Environment::get_instance()->inActiveGPUId(prev_id,applicationName);	
 	}//return false;
 	//std::cout<< "I am" << applicationName << " and This is my MASKED ocl device number:++++++++++++++++++++++++++++++ " << prev_id<<std::endl;
 	std::string dev_type=Environment::get_instance()->node_info->DevicesInfo.at(prev_id)->deviceType;
	if (strcasecmp(dev_type.c_str(), "CL_DEVICE_TYPE_CPU")==0)	 
	{
		masked_cpu+=num;

	}else if (strcasecmp(dev_type.c_str(), "CL_DEVICE_TYPE_GPU")==0)	 
	{
		masked_gpu+=num;
	} 
	return prev_id;
}

std::string mappingHeuristics::min_val(int cpu, int gpu)
{
	if(cpu<gpu) return "CL_DEVICE_TYPE_CPU";
	else return "CL_DEVICE_TYPE_GPU";
}

void mappingHeuristics::unMaskComponentDevice(std::string applicationName, int num)
{
	int prev_id=Environment::get_instance()->getComponentGPUDevice(applicationName);
 	//if(prev_id==-1) return false;
 	//std::cout<< "This is my ocl device number for "<< applicationName<<" I am going to release :++++++++++++++++++++++++++++++ " << prev_id<<std::endl;
 	std::string dev_type=Environment::get_instance()->node_info->DevicesInfo.at(prev_id)->deviceType;
	if (strcasecmp(dev_type.c_str(), "CL_DEVICE_TYPE_CPU")==0)	 
	{
		masked_cpu =masked_cpu -num;

	}else if (strcasecmp(dev_type.c_str(), "CL_DEVICE_TYPE_GPU")==0)	 
	{
		masked_gpu =masked_gpu -num;
	} 
	Environment::get_instance()->inactivedeallocateGPUComponent(applicationName);

	//int numd =(Environment::get_instance()->gpuMappingArray[prev_id]->total_app);
	//std::cout<< "decreased val for " << prev_id<<"This is the new  total number of allocated to GPU : "<< numd<<std::endl;
	//  	std::cout<< "decreased val for for max number on this device : " << "for" << prev_id << ":"<<get_max_ocl_task_per_device(prev_id)<<std::endl;
}

int mappingHeuristics::suspend_application(std::string applicationName)
{
	

//all worker is gone off the whole applocation must be suspended.
	int pIdPosition= applicationName.find("/");
	std::string processId = applicationName.substr(0,pIdPosition);
	if(registered_apps[processId]->activation_status)
	{   int cl_id=try_masking(applicationName);
		if(cl_id>=0)  return cl_id;
		else
		{
			registered_apps[processId]->activation_status=false; 									
			set<string>::iterator m_iter = suspended_apps.find(processId);

		    if(m_iter==suspended_apps.end())
		    {
					suspended_apps.insert(processId);
				//	std::cout << "**************************I have been suspended *****************************************************"<<std::endl;
			}
		}
	}
	Environment::get_instance()->releaseResources(processId);
	return -1;
}

int mappingHeuristics::get_gpu_device(std::string applicationName)
{
	int cl_id =findBestGPUDevice(applicationName);
	std::cout << "************* This is the name I allocate the GPU:"<<applicationName<< std::endl;

	//int pIdPosition= applicationName.find("/");
	//std::string processId = applicationName.substr(0,pIdPosition);
	if(cl_id>=0) 
	{
		Environment::get_instance()->allocateGPUId(cl_id,applicationName);	
		//std::cout<< "AppName :" << applicationName<< " has been allocated to  " << cl_id;
	}
	else//if(cl_id<0) 
	{
		cl_id =suspend_application(applicationName);
		//std::cout<< "AppName :" << applicationName<< " has been masked to  " << cl_id;
		//if(cl_id>=0) Environment::get_instance()->inActiveallocateGPUId(cl_id,processId);	
		//if(cl_id<0) 
		//std::cout<< "error: cannot detect any device to allocate the opencl node"<< std::endl;
    //	std::cout << "++++++++++++this is my activation status" << (registered_apps.at(processId))->activation_status<<std::endl;
    	//registered_apps[processId]->activation_status=false; 
    	
    //	set<string>::iterator m_iter = suspended_apps.find(processId);

    //	if(m_iter==suspended_apps.end())
    //	{
      //  	suspended_apps.insert(processId);
        //	std::cout << "**************************I have been suspended *****************************************************"<<std::endl;
    	//}
	}
	return cl_id;
}
//try to move to gpu still left
int  mappingHeuristics::findBestGPUDevice(std::string applicationName/*, std::string cl_dev_type_constraint="CL_DEVICE_TYPE_ALL"*/)
{	
	// no need for this if becases if any one need to specify he can directly use the finddevbytype
	//if((strcasecmp("CL_DEVICE_TYPE_ALL", cl_dev_type_constraint.c_str()) == 0))
	//{

	int pIdPosition= applicationName.find("/");
	std::string processId = applicationName.substr(0,pIdPosition);
	if(registered_apps[processId]->activation_status)
    {
	    std::vector<std::string> sortedApps= prioritySort(currently_selected_apps);

	 	int cl_id =findBestGPUDeviceByType(applicationName, "CL_DEVICE_TYPE_GPU"); // try to move to gpu
	    
	    if (cl_id==-1)// try to swap 
	    {   int i=0;
	    	for(std::vector<std::string>::reverse_iterator iter=sortedApps.rbegin(); iter!=sortedApps.rend();++iter) //iterator over potential victim
	    	{
	    		if (strcasecmp((*iter).c_str(), processId.c_str())==0) // this can also be used for swapping between two ocl node of and application
	    		{
	    		//	std::cout<< "I end up to my selef. My number is:*********************** "<<  i << " My process Id is:" << processId<< std::endl;;

	    			break; // this one must be deactivated at the end in the build actuator
	    		} 
	    		i++;
	    		if(((registered_apps[processId]->priority)<(registered_apps[*iter]->priority)) && registered_apps[*iter]->activation_status)
		    	{	
		    		for(int i=0; i<Environment::get_instance()->node_info->DevicesInfo.size(); i++) 
		    		{
		    			std::string dev_type=Environment::get_instance()->node_info->DevicesInfo[i]->deviceType;
		    			if (strcasecmp(dev_type.c_str(), "CL_DEVICE_TYPE_GPU")==0)
						{
							if((Environment::get_instance()->gpuMappingArray[i]->allocatedapplications.find((*iter)))!=(Environment::get_instance()->gpuMappingArray[i]->allocatedapplications.end()))
							{
								for(std::set<std::string>::iterator it=(Environment::get_instance()->gpuMappingArray[i]->allocatedComponents).begin();
								it!=(Environment::get_instance()->gpuMappingArray[i]->allocatedComponents).end(); ++it )
								{
							//		std::cout<< "this is the node name ********************"<<(*it)<< std::endl;
									std::string node_address=(*it);
									int victimIdPosition= (*it).find("/");
									std::string victimId = (*it).substr(0,victimIdPosition);
									if (strcasecmp((*iter).c_str(), victimId.c_str())==0)
									{
										Environment::get_instance()->deallocateGPUComponent((*it));
										cl_id= findBestGPUDeviceByType(applicationName, "CL_DEVICE_TYPE_GPU");
										int victim_device_id=findBestGPUDeviceByType(node_address, "CL_DEVICE_TYPE_CPU");
										
										if(victim_device_id==-1)
										{
											//registered_apps[victimId]->activation_status=false; 
											
											//set<string>::iterator m_iter = suspended_apps.find((*iter));
		                				//	if(m_iter==suspended_apps.end())
		                				//	{
		                    				//	suspended_apps.insert((*iter));
		                    				//	std::cout << "**************************I have been suspended *****************************************************"<<std::endl;
		                					//}

											// this means that the whole application must loose its
											// resources by release resource and next time it has been awaken it can gain its resource
											//std::string vicitimed = (*iter);
			    						//	std::cout<< "===============================================I "<< processId 
			    						//	<<" am vicitimisig low priority======================================" << vicitimed << "and " 
			    							//<< node_address<<std::endl;
			    							suspend_application(node_address);
											//Environment::get_instance()->releaseResources((*iter));
										}
										else
										{
										std::cout<< "THIS IS ME^^^^^^^^^^^^^^^^^^^^^^^^" << applicationName<<std::endl;
										std::cout<< " AND THIS IS VICTIM^^^^^^^^^^^^^^^^^^^^^^^^" << node_address<<std::endl;
										//	std::cout<< "THis is the device id************************"<< victim_device_id<<std::endl;
											Environment::get_instance()->allocateGPUId(victim_device_id,node_address);
											//int a = 10;
											stringstream ss;
											ss << victim_device_id;
											string str = ss.str();
											std::string field_name("Device_Number");
											//std string number = static_cast<ostringstream*>( &(ostringstream() << victim_device_id) )->str();
											JSONNode jn = libjson::parse(registered_apps[(*iter)]->actuator);
											std::string node_address=(*it);
											int victimIdPosition= (*it).find("/");
											std::string victime_component_name = node_address.substr(victimIdPosition);
										//	std::cout<< "This is the fouund address:****************************"<< victime_component_name<< std::endl;
		        							editActuator<std::string>(jn, victime_component_name,field_name, str);

		        							registered_apps[(*iter)]->actuator=jn.write_formatted();
		        							//std::cout<< "this will be send to :   "<< (*iter)<< " : "<< registered_apps[(*iter)]->actuator<< std::endl;
		        							set<string>::iterator m_iter = modified_actuators_apps.find((*iter));
		                					if(m_iter==modified_actuators_apps.end())
		                    					modified_actuators_apps.insert((*iter));

										}
										break;
									}
								}
							}
						}
		    	}	}
	    	}

	    }
	    if (cl_id ==-1) 
	    {
	    	cl_id = findBestGPUDeviceByType(applicationName, "CL_DEVICE_TYPE_CPU");
	    	if(cl_id==-1)
	    	{
		    	for(std::vector<string>::reverse_iterator iter=sortedApps.rbegin(); iter!=sortedApps.rend();++iter) //iterator over potential victim
		    	{
		    		if (strcasecmp((*iter).c_str(), processId.c_str())==0)
		    		{
		    //			std::cout<< "I end up to my selef. My id is:*********************** "<<  cl_id << " My process Id is:" << processId<< std::endl;;
		    			//std::string vicitimed = (*iter);
			    		//std::cout<< "===============================================I am vicitimisig myself======================================" << processId<<std::endl;
		    			//registered_apps[processId]->activation_status==false; 
		    			//Environment::get_instance()->releaseResources(processId); //it will be released at the end
		    			
		    			//set<string>::iterator m_iter = suspended_apps.find(processId);
			            //if(m_iter==suspended_apps.end())
	        			//	suspended_apps.insert(processId);
		    			
		    			break;
		    		} 
		    		else if(((registered_apps[processId]->priority)<(registered_apps[*iter]->priority)) && registered_apps[*iter]->activation_status)
			    	{	
			    		//registered_apps[(*iter)]->activation_status=false; 
			    		std::string vicitimed = (*iter);
			   // 		std::cout<< "===============================================I "<< processId<<" am vicitimisig after cpu======================================" << vicitimed<<std::endl;
			    		
			    		//Environment::get_instance()->releaseResources((*iter));
						//set<string>::iterator m_iter = suspended_apps.find((*iter));
	    				//if(m_iter==suspended_apps.end())
	        			//	suspended_apps.insert((*iter));
	        			std::string cmp_name= Environment::get_instance()->getComponent(vicitimed, "CL_DEVICE_TYPE_CPU");
	        			suspend_application(cmp_name);
	        			cl_id= findBestGPUDeviceByType(applicationName, "CL_DEVICE_TYPE_CPU");
			    	}
			    }

			}  

	    }
	//    std::cout << "**************************I must be here*****************************************************"<< processId<<std::endl;
	    return cl_id;
    }
    else 
    {
  //  	std::cout << "**************************I am already suspended *****************************************************"<< processId<<std::endl;
    	return -1;
    //	return findBestGPUDeviceByType(applicationName, cl_dev_type_constraint);
    }
};
// try to swap a high priority component to one of the component with low priority and less cpu total time
/*bool mappingHeuristics::heursiticCPUSwap(int i, std::map<string, app_info*>& registerd_apps, std::vector<std::string>& current_apps)
{
	for(std::vector<string>::iterator iter = evaluatedApplications.rbegin(); i!=evaluatedApplications.rend(); ++iter)
	{
		if(strcasecmp(applicationName.c_str(), (*iter).c_str() == 0)
		{
			return false;
		}
		else
		{
			findBestCPUCore
		}
	}
}*/

/*bool mappingHeuristics::improveBtnNode(int chosen, std::map<string, app_info*>& registerd_apps, std::vector<std::string> &current_apps)
{
	JSONNode btn_node(JSON_NULL);
    btn_node= libjson::parse(registerd_apps[current_apps.at(chosen)]->btn_node);  
    std::string chosenComponent =btn_node.at_nocase("Name");

    // one means gpu zero means cpu
    if(Environment::get_instance()->get_device_type(chosenComponent)==0)
    {
    	// try to move to gpu
    	// if improved return those needed to change
    }
    // else try to swap
    // else try to enlightened
    // else try to put sleep
    Environment::get_instance()->get_least_busy_GPU();
    if(Environment::get_instance()->get_device_type(chosenComponent)==1)
    {

    }


	for(int i=current_apps.size()-1; i>=0; i--)
	{
		if(i==chosen)
		{
			return false;
		}
		else
		{
		}
	}

//	for(std::iter)
};*/
// try to swap a high priority component with the one low priority component running om gpu.
/*bool mappingHeuristics::heursiticGPUSwap(int chosen, std::map<string, app_info*>& registerd_apps, std::vector<std::string> &current_apps)
{
	
};
//move to better cpu with less totalTime
bool mappingHeuristics::heursiticCPUMove(std::string applicationName, std::string componentName)
{

};
//move to better GPU with with less ocupide or from cpu to GPU
bool mappingHeuristics::heursiticGPUMove(std::string applicationName , std::string componentName)
{

};
*/

int  mappingHeuristics::findBestGPUDeviceByType(std::string applicationName, std::string cl_dev_type_constraint)
{
	int pIdPosition= applicationName.find("/");
	std::string processId = applicationName.substr(0,pIdPosition);
	//std::cout<< "this is the process ID:" <<processId<<std::endl;

	int id=-1;
	float maxProportion=1;
	for(int i=0; i< Environment::get_instance()->node_info->DevicesInfo.size(); ++i)
	{  
	  std::string dev_type=Environment::get_instance()->node_info->DevicesInfo[i]->deviceType;
	  if((!cl_dev_type_constraint.empty()) && (strcasecmp(cl_dev_type_constraint.c_str(), dev_type.c_str()) == 0))
	  { 
	  	//int num =(Environment::get_instance()->gpuMappingArray[i]->total_app);
	  //	std::cout<< "I am " << i<<"This is the new  total number of allocated to GPU : "<< num<<std::endl;
	  //	std::cout<< "this is the max number on this device : " << "for" << i << ":"<<get_max_ocl_task_per_device(i)<<std::endl;
	    float proportion = (1.0 *(Environment::get_instance()->gpuMappingArray[i]->total_app))/get_max_ocl_task_per_device(i);
	    if(proportion<maxProportion)
	    {
	      maxProportion=proportion;
	      id =i;
	    }
	  }
	}
	// now the task must be added to the device
	// in order to do so i need to find it in the std::map increase its application and increase also the total app as well
	

	return id;  
};
// allocate cpu id must change it must update must of the time and if it is not exist add to it
int mappingHeuristics::findBestCPUCore(std::string applicationName, double serviceTime, int prev_id)
{
	// Environment::get_instance()->UpdateCPUMappingArray(applicationName,  serviceTime);
    //if(prev_id==-1)
  //  {
  		int pIdPosition= applicationName.find("/");
		std::string processId = applicationName.substr(0,pIdPosition);
		//std::cout<< "this is the process ID:" <<processId<<std::endl;
	    // prev_Id infpo must be added
	//	std::cout<< "serviceTime:" <<serviceTime <<std::endl;
	    if(serviceTime==-1 || serviceTime==0) serviceTime=1;
	    int id=-1;
	    double minProportion=-1;//((Environment::get_instance()->cpuMappingArray[id])->total_time);
	    //std::cout<<"min proportion: "<<minProportion<<std::endl;
	    for(int i=3; i< ((Environment::get_instance()->node_info->numCPU)+2); ++i)
	    {  
	    	int indx = (i%(Environment::get_instance()->node_info->numCPU));
	     	std::map<string, int>::iterator num_iter = ((Environment::get_instance()->cpuMappingArray[indx])->allocatedapplications).find(processId);
	     	int discaurage=0;
	     	if(num_iter!=(Environment::get_instance()->cpuMappingArray[indx])->allocatedapplications.end())
	     		discaurage=num_iter->second;
	        if (discaurage>1) discaurage= discaurage +(1.0-(1.0/(discaurage)));
	     	double current_proportion =(Environment::get_instance()->cpuMappingArray[indx]->total_time) + discaurage*((Environment::get_instance()->cpuMappingArray[indx]->total_time));
	     	if(minProportion==-1 || current_proportion< minProportion)
	      
	      	{
	        	minProportion=current_proportion;
	        	id=indx;
	      	}
	    }
	   
	    
	    return id;  
//	}
//	else 
//		return prev_id;
};

