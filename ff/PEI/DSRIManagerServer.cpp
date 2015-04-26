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
 
#include <ff/PEI/DSRIManagerServer.hpp>
//using namespace std;
//namespace ff
//class DSRIManagerServer{
//private:
  //mappingHeuristics * m_heuristic;
//public:

  DSRIManagerServer::DSRIManagerServer()
  {
      //DLT_STATUS=true;
      srand (time(NULL));
  }

  void DSRIManagerServer::releaseResources(std::string applicationName)
  {
     Environment::get_instance()->releaseResources(applicationName);  
  }

	/*WorkerInfo * DSRIManagerServer::getWorkerInfo(const JSONNode& jn)
  {//yes
  		int wid=-1;
  		ticks eTicks=0;
  		STATUS s=OFF;
      double serviceTime=0;
  		JSONNode::const_iterator it =jn.find_nocase("Local_ID");
  		if(it->as_string()!=""){
  			wid = it->as_int();
  		}
  		it =jn.find_nocase("Effective_Ticks");
  		if(it->as_string()!=""){
  			eTicks = it->as_int();
  		}

      it =jn.find_nocase("Service_Time");
      if(it->as_string()!=""){
        serviceTime = it->as_float();
      }

  		it =jn.find_nocase("Status");
  		if(it->as_string()!="" && it->as_int()==1){
  			s = ON;
  		}
  		return new WorkerInfo(wid,eTicks,0,serviceTime,0,s);
	};*/

	//STATUS DSRIManagerServer::get_masking(const JSONNode & jn, const JSONNode &wjn)
   // {//yes
      	//evaluate the mobj and get the decision about masking
     // 	JSONNode::const_iterator it =wjn.find_nocase("Effective_Ticks");
     // 	if(it->as_string()!="" && it->as_int()!=0) return ON;
  	//	else return OFF;
//	};


	/*ticks DSRIManagerServer::getFarmEffectiveTick(JSONNode& workersActuators, unsigned long em_send_task, unsigned long em_recieve_task)
    {//yes
  		ticks estimatedFarmTime= 0;
      //JSONNode::const_iterator it =sensors.find_nocase("Workers");
      
    for(unsigned int i = 0; i < workersActuators.size(); ++i)
    {
        ticks tmp=0;
        JSONNode jn =workersActuators[i].as_node();
        //std::cout<<"THIS IS ERROR"<<std::endl;
        //std::cout<< jn.write_formatted()<<std::endl;
        
        JSONNode::const_iterator it =jn.find_nocase("Effective_Ticks");
        if(it!=jn.end() && it->as_string()!="")
        {
          tmp = static_cast<ticks>(it->as_int());
        } 
        if (tmp> estimatedFarmTime)
        {
          estimatedFarmTime=tmp;
        }
        workersActuators[i].erase(workersActuators[i].find_nocase("Effective_Ticks"));
    }	
 		// std::cout<<"worker estimated Time: "<<estimatedFarmTime<<std::endl;
  	if(workersActuators.size()>0) {em_send_task/=workersActuators.size();}
  	ticks tempTime = (estimatedFarmTime*em_send_task);
  	if(em_recieve_task>0){
    	estimatedFarmTime=(tempTime/em_recieve_task); 
  	}
  		//std::cout<< "em_recieve_task: "<<em_recieve_task<<std::endl;
  		//std::cout<<"em_send_task: "<<em_send_task<<std::endl;
 		// std::cout<<"farm estimated time: "<< estimatedFarmTime<<std::endl;
  	return estimatedFarmTime;
	};*/


  double DSRIManagerServer::getFarmEffectiveTime(JSONNode& workersActuators, unsigned long em_send_task, unsigned long em_recieve_task)
  {//yes


    double estimatedFarmTime= 0;
     // JSONNode::const_iterator it =sensors.find_nocase("Workers");
      
      for(unsigned int i = 0; i < workersActuators.size(); ++i)
          {
            double tmp=0;
            JSONNode jn =workersActuators[i].as_node();
            JSONNode::const_iterator it =jn.find_nocase("Service_Time");
            if(it!=jn.end() && it->as_string()!="")
            {
              tmp = it->as_float();
            } 
            if (tmp> estimatedFarmTime)
            {
              estimatedFarmTime=tmp;
            }
            workersActuators[i].erase(workersActuators[i].find_nocase("Service_Time"));
          }
      
    // std::cout<<"worker estimated Time: "<<estimatedFarmTime<<std::endl;
      if(workersActuators.size()>0) {em_send_task/=workersActuators.size();}
      double tempTime = (estimatedFarmTime*em_send_task);
      if(em_recieve_task>0){
        estimatedFarmTime=(tempTime/em_recieve_task); 
      }
      //std::cout<< "em_recieve_task: "<<em_recieve_task<<std::endl;
      //std::cout<<"em_send_task: "<<em_send_task<<std::endl;
    // std::cout<<"farm estimated time: "<< estimatedFarmTime<<std::endl;
      return estimatedFarmTime;
    };


	/*ticks DSRIManagerServer::getEffectiveTick(const JSONNode& jn)
    {//yes
  		ticks eSvcTicks=-1;
  		if(jn.type()!=JSON_NULL){
  			JSONNode::const_iterator it =jn.find_nocase("Total_SVC_Ticks");
  			JSONNode::const_iterator iter =jn.find_nocase("Total_Number_Of_Tasks");
      		if(it!=jn.end() &&  iter!=jn.end() && it->as_string()!="" && iter->as_string()!="" && iter->as_int()!=0)
          {
      			eSvcTicks= ((it->as_int())/(iter->as_int()));
      		}
  		}
  		return eSvcTicks;
	};*/

    double DSRIManagerServer::getEffectiveTime(const JSONNode& jn)
    {//yes
        double serviceTime=0;
        if(jn.type()!=JSON_NULL)
        {
            JSONNode::const_iterator it =jn.find_nocase("Total_Component_Active_Time");
            JSONNode::const_iterator iter =jn.find_nocase("Processed_Tasks");
            if(it!=jn.end() &&  iter!=jn.end() && it->as_string()!="" && iter->as_string()!="" && iter->as_int()!=0)
            {
                serviceTime= ((it->as_float())/(iter->as_int()));
            }
        }
      return serviceTime;
  };

	/*ticks DSRIManagerServer::getEffectiveLbTick(const JSONNode& jn)
    { //yes
		ticks eSvcTicks=-1;
  		if(jn.type()!=JSON_NULL){
  			JSONNode::const_iterator it =jn.find_nocase("Total_SVC_Ticks");
  			JSONNode::const_iterator iter =jn.find_nocase("Total_Number_Of_Received_Tasks");
      		if(it!=jn.end() &&  iter!=jn.end() && it->as_string()!="" && iter->as_string()!="" && iter->as_int()!=0){
      			eSvcTicks= ((it->as_int())/(iter->as_int()));
      		}
  		}
  		return eSvcTicks;
	};*/


  double DSRIManagerServer::getEffectiveLbTime(const JSONNode& jn)
  { //yes
    double serviceTime=-1;
      if(jn.type()!=JSON_NULL){
        JSONNode::const_iterator it =jn.find_nocase("Total_Component_Active_Time");
        JSONNode::const_iterator iter =jn.find_nocase("Ch_In");
          if(it!=jn.end() &&  iter!=jn.end() && it->as_string()!="" && iter->as_string()!="" && iter->as_int()!=0){
            serviceTime= ((it->as_float())/(iter->as_int()));
          }
      }
      return serviceTime;
  };

double DSRIManagerServer::get_arrival_rate(double elapsed_time, unsigned long in_count , unsigned long out_count, double current_arr_rate /*std::vector<WorkerInfo*> wsTime*/)
{
  double arrRate=current_arr_rate;
  if(in_count>0){
    double throughput = (1.0*out_count)/elapsed_time;
    if (throughput==0) arrRate = (1.0*in_count)/elapsed_time;
    else{
      double income = (1.0*in_count)/elapsed_time;
      if (income/throughput>1) arrRate =1.2/throughput;
      else if(income/throughput<=1) arrRate =0.9/throughput;
    }
  }
  /*std::vector<WorkerInfo*>::iterator it = wsTime.begin();
  double arrRate=(*it)->serviceTime;
  ++it;
  while (it!=wsTime.end()){
    if(arrRate==-1) arrRate=(*it)->serviceTime;
   else if(((*it)->serviceTime)!=-1 && (arrRate > ((*it)->serviceTime))) {arrRate=(*it)->serviceTime;}
    ++it;
  }*/
  return arrRate;
};

//void adjust_priority(vector<string> app_names)
//{
 // mappingHeuristics::get_instance()->calculate_age(app_names);
//}

JSONNode DSRIManagerServer::get_maskArray(std::vector<STATUS> msk)
{
  JSONNode marr(JSON_ARRAY);
  marr.set_name("Masking_Array");
  for(std::vector<STATUS>::iterator it = msk.begin(); it!=msk.end(); ++it)
  {
    marr.push_back(JSONNode("",*it));
  }
  return marr;
}
/*JSONNode& DSRIManagerServer::find_node(JSONNode& actuator, std::string node_name)
{
    std::vector<string> seperated_names=DSRIManagerServer::getNodeName(std::string node_name);
    std::string my_name =seperated_names[0];

    //now trace
}
// this is the reinforcement learning. it returns the id of the app that must be changed
std::vector<string> DSRIManagerServer::getNodeName(std::string cmpAddress)
{
    int pIdPosition=  cmpAddress.find_first_of("/");
    std::string remainingString = cmpAddress.substr(pIdPosition+1);
    std::vector<string> seperated_names;
    while(true)
    {   
        pIdPosition=  remainingString.find_first_of("/");
        std::string first_part= remainingString.substr(0,pIdPosition);
        remainingString= remainingString.substr(pIdPosition+1);
        pIdPosition=  remainingString.find_first_of("/");
        std::string second_part= remainingString.substr(0,pIdPosition);
        seperated_names.push_back("/" + first_part + "/" + second_part);
        if(pIdPosition!=std::string::npos) break;

    }
    return seperated_names;
}*/


/*int DSRIManagerServer::selectApp(std::vector<std::string>& current_apps)
{
    std::vector<priorityLoadBound*> calculatedboundaries;

    double min =0.0;
    //normalized priority
    double totalPriority=0.0;
    for(std::vector<std::string>::iterator iter=current_apps.begin(); iter!=current_apps.end(); ++iter)
    {
        //std::map<string, app_info*>::iterator app_iter = registered_apps.find(*iter);
        totalPriority += (registered_apps.at(*iter)->priority);
    }
    // generating priority based weight to select app
    for (std::vector<std::string>::iterator it = current_apps.begin() ; it != current_apps.end(); ++it)
    {
        double x= (((registered_apps.at(*it)->priority)*1.0)/totalPriority);
        calculatedboundaries.push_back((new priorityLoadBound(min, (min + x))));
        min += x;
    }

    double ld = ((double) rand() / (RAND_MAX));
    for(int i=0; i<calculatedboundaries.size(); i++)
    {
        if((ld>=calculatedboundaries.at(i)->min) && (ld< calculatedboundaries.at(i)->max))
        {
            return i;
        }
    }
    return -1;

}*/

/*int DSRIManagerServer::selectAction()
{  
    return  ((double) rand() / reinforcement_awards.size());
}*/
// this function will return the name of the sensor in the databasefile on the disk
void DSRIManagerServer::load_sensor_file(std::string app_name, std::string sensor_file)
{
    JSONNode sensors(JSON_NULL);
    sensors= libjson::parse(sensor_file);
    std::string filename =sensors.find_nocase("Sensor_Output_Filename_Name")->as_string();
    sensors.erase(sensors.find_nocase("Sensor_Output_Filename_Name"));
    registered_apps[app_name]->priority=sensors.find_nocase("Priority")->as_int();
    registered_apps[app_name]->sensor= sensors.write_formatted();
    std::size_t found;
    found =filename.find("/");
    while(found!=std::string::npos)
    {
        filename.replace(found, 1, "_");
        found =filename.find("/");
    
    }
    registered_apps[app_name]->application_file_id= filename;
    load_sensor(app_name);
    



};

void  DSRIManagerServer::deregister_maskFarm(std::string processId)
{
  mappingHeuristics *mh = new mappingHeuristics();
  mh->deregister_masked(processId); 
}
void DSRIManagerServer::flush_sensor_file(std::string app_name)
{
    JSONNode sensors(JSON_NULL);
    sensors= libjson::parse(registered_apps[app_name]->sensor);
    sensors.erase(sensors.find_nocase("Sensor_Output_Filename_Name"));
    registered_apps[app_name]->sensor= sensors.write_formatted();
    if(registered_apps[app_name]->has_useless)
    {
      mappingHeuristics * mh = new mappingHeuristics();
      JSONNode jn = libjson::parse(registered_apps[app_name]->sensor);
      for(std::set<std::string>::iterator  i_iter=registered_apps[app_name]->useles_node_address.begin();
       i_iter!=registered_apps[app_name]->useles_node_address.end(); ++i_iter)
      {
        mh->editActuator<int>(jn, (*i_iter), "End_Received", 0);
      }
      registered_apps[app_name]->sensor= jn.write_formatted();
      registered_apps[app_name]->useles_node_address.clear();
    }
    flush_sensor(app_name);
};
   
void DSRIManagerServer::update_application_information(std::string app_name)
{
    JSONNode sensors(JSON_NULL);
    sensors= libjson::parse(registered_apps[app_name]->sensor);  
    JSONNode btn_path(JSON_NULL);    
    btn_path=DSRIManagerServer::find_buttleneck_path(app_name, sensors, sensors.find_nocase("Elapsed_Time")->as_float());
    registered_apps[app_name]->btn_path=btn_path.write_formatted();
    JSONNode btn_node(JSON_NULL);    
    btn_node=DSRIManagerServer::find_buttleneck_node(btn_path);
    registered_apps[app_name]->btn_node =btn_node.write_formatted(); 
    std::cout<< registered_apps[app_name]->btn_node<<std::endl;
    //Environment::get_instance()->updateActualCPULoad();  
}

bool DSRIManagerServer::improve_btn_node(std::string app_name)
{
        
    JSONNode btn_node(JSON_NULL);    
    btn_node=libjson::parse(registered_apps[app_name]->btn_node);
    //int e_val = btn_node.at_nocase("Efficiency_Rate").as_int();
    std::set<std::string> ::iterator it = (registered_apps[app_name]->useles_node_address).find(btn_node.at_nocase("Node_Address").as_string());
    if((it==(registered_apps[app_name]->useles_node_address).end()) && 
      (!(DSRIManagerServer::is_mask((app_name +btn_node.at_nocase("Node_Address").as_string())))) /*&& (e_val!=USELESS_NODE)*/)
    {
      JSONNode::const_iterator j_iter = btn_node.find_nocase("Component_Type");
      if(j_iter!=btn_node.end())
      {
          std::string node_type=j_iter->as_string();
          if(strcasecmp(node_type.c_str(), "Hsequential")==0)
          {
              j_iter = btn_node.find_nocase("Node_Address");
              if(j_iter!=btn_node.end())
              {
                  std::string node_name=j_iter->as_string();
                  mappingHeuristics* m_heuristic= new mappingHeuristics();
                  if(m_heuristic->moveToGPU(app_name + node_name))
                  {
                      registered_apps[app_name]->DLTUpdateNeeded=true;
                    //  registered_apps[app_name]->DLTSwitchTransition=SWITCH_THRESHOLD;
                      return true;
                  }
              }
              
          }
      }
    }
    return false;

}

std::string DSRIManagerServer::get_action(std::string action)
{
    JSONNode act(JSON_NODE);
    //act.push_back(JSONNode("Node_Address", ));
    act.push_back(JSONNode("Action", action));
    return act.write_formatted();
}

void DSRIManagerServer::environmentConstraint(std::string env_constraint_address)
{
    
  std::string env_const_file = read_json_file(env_constraint_address);
  if(!(env_const_file.empty()))
  {
    JSONNode env_const = libjson::parse(env_const_file);  
    DSRIManagerServer::construct_constraint(env_const);
    /*
    JSONNode::const_iterator r_iter = env_const.find_nocase("CPU_Limit");
    if(r_iter!=env_const.end()) 
    {
      Environment::get_instance()->set_cpu_limit(r_iter->as_int());
    }
    r_iter = env_const.find_nocase("GPU_Limit");
    if(r_iter!=env_const.end()) 
    {
      Environment::get_instance()->set_gpu_limit(r_iter->as_int());
    }
    r_iter = env_const.find_nocase("Component_Switch_Mode");
    if(r_iter!=env_const.end()) 
    {
      if(strcasecmp(r_iter->as_string().c_str(), "ad-hoc")==0)
        sMode =AD_HOC;
      else 
        sMode=AVERAGE;
    }
    r_iter = env_const.find_nocase("Priority_Type");
    if(r_iter!=env_const.end()) 
    {
      if(strcasecmp(r_iter->as_string().c_str(), "variable")==0)
        pr =VARIABLE;
      else 
        pr=FIX;
    }
    r_iter = env_const.find_nocase("Priority_Period");
    if(r_iter!=env_const.end()) 
    {
        pr_period =r_iter->as_float();
    }
    r_iter = env_const.find_nocase("DLT_UPdate_Period");
    if(r_iter!=env_const.end()) 
    {
        dlt_period =r_iter->as_float();
    }*/

  }
}

void DSRIManagerServer::construct_constraint(JSONNode& constraint)
{
    JSONNode::const_iterator i = constraint.begin();
    while (i != constraint.end()){
        // recursively call ourselves to dig deeper into the tree
        if (i -> type() == JSON_NODE)
        {
          if(strcasecmp((i->name()).c_str(), "Device") == 0 )
          {
            JSONNode dev = i->as_node();
            JSONNode::const_iterator n_iter = dev.find_nocase("Name");
            if(n_iter!=dev.end())
            {
              if (strcasecmp((n_iter->name()).c_str(), "GPU") == 0 )
              {
                JSONNode::const_iterator d_iter = dev.find_nocase("Maximum_BB_Per_Device");
                if(n_iter!=dev.end())
                Environment::get_instance()->set_gpu_limit(d_iter->as_int());
              }
              else if (strcasecmp((n_iter->name()).c_str(), "CPU") == 0 )
              {
                JSONNode::const_iterator d_iter = dev.find_nocase("Maximum_BB_Per_Device");
                if(n_iter!=dev.end())
                Environment::get_instance()->set_cpu_limit(d_iter->as_int());
              }
            }
          }
          else if(strcasecmp((i->name()).c_str(), "Application") == 0 )
          {
               std::cout << "Warning: the constraint for specific application has not been implemented yet. Please use overal constraint which can be applied to all applications." << std::endl;
          }
        }
        else
        {
          if(strcasecmp((i->name()).c_str(), "Maximum_BB_Per_Device") == 0 )
          {
            int dev_block= i->as_int();
            Environment::get_instance()->set_cpu_limit(dev_block);
            Environment::get_instance()->set_gpu_limit(dev_block);
          }
          else if(strcasecmp((i->name()).c_str(), "Component_Switch_Mode") == 0 )
          {
            if(strcasecmp(i->as_string().c_str(), "ad-hoc")==0)
              sMode =AD_HOC;
            else 
              sMode=AVERAGE;
          }
          else if(strcasecmp((i->name()).c_str(), "Priority_Policy") == 0 )
          {
            if(strcasecmp(i->as_string().c_str(), "variable")==0)
              pr =VARIABLE;
            else 
              pr=FIX;
          }
          else if(strcasecmp((i->name()).c_str(), "Damping_Ratio") == 0 )
          {
              pr_period =i->as_float();
               dlt_period =i->as_float();
          }
        }      
        //increment the iterator
        ++i;
    }
}

std::string DSRIManagerServer::build_actuators(std::string app_name)
{ 
   // std::string app_name = sensors.at_nocase("Application_Name").as_string();
    //Environment::get_instance()->releaseResources(app_name); no need
    JSONNode sensors(JSON_NULL);
    sensors= libjson::parse(registered_apps[app_name]->sensor);  
    JSONNode btn_path(JSON_NULL);    

    JSONNode actuators= DSRIManagerServer::parse_sensors(sensors, app_name);
    if(!(registered_apps[app_name]->activation_status))
    {
      std::cout<< "+++++++++++++++++++++++++ No resource found. Temporarily suspending"<< std::endl;
      Environment::get_instance()->releaseResources(app_name);
    }
   // actuators.erase(actuators.find_nocase("Effective_Ticks"));
    actuators.erase(actuators.find_nocase("Service_Time"));

    // these line will go to the function which call this build actuator that function recieved all sensors and run this. and at the end recived 
    // the vector of actuators
    // application_utilisation_info* ap_inf = new application_utilisation_info();
   // ap_inf->overalapplicationawards = DSRIManagerServer::find_buttleneck_node(btn_path).at_nocase("Efficiency_Rate").as_float();
   // ap_inf->action_rewards.push_back(btnRate);
   // reinforcement_awards[(sensors.at_nocase("Node_Address").as_string()+ cServiceTime_iter->as_string())]=ap_inf;
    //for(std::map<string, application_utilisation_info*>::iterator iter =reinforcement_awards.begin(); iter!= reinforcement_awards.end(); ++iter)
    //{  
     // std::cout<< "Name:" <<  iter->first<<std::endl;
     // application_utilisation_info* ap_inf= (application_utilisation_info*) iter->second;
     // std::cout<< "overal_rewards:"<< ap_inf->overalapplicationawards<<std::endl;
     // for (std::vector<double>::iterator it= (ap_inf->action_rewards).begin(); it!=(ap_inf->action_rewards).end(); ++it)
     // {
      //  std::cout << "value:"<< (*it) <<std::endl;
     // }
   // }

    return actuators.write_formatted();
  };

//action get_action(){
      // this function will select one of the three(path or node distribution or nothing) heuristic and will give revard to each of them based on the analyse of
      // the next run. the desisions are
      //1)either do nothing
      //2) or run the dlt for buttleneck path
      //3) thry to reallocate the buttlenck node:
        //3-1)or reassignment of cpu the butleneck node
      // 3-2)or reassignment of ocl  butleneck node to GPU
      // 3-3)exchange GPU with older
      //3-4)put older running on GPU to cPU
      //3-5)put older on sleep
      // this function returns the action to the build_actuator. the returned action
      // is used in parse sensor to build the required action for the buttleneck pass or butleneck node
//}
std::vector<STATUS> DSRIManagerServer::get_masking(std::string farm_address, int numWorkers, std::string app_name)
{
   std::vector<STATUS> maskVector(numWorkers, ON);
   for(std::map<std::string , farmStructure*>::iterator it=farmMask.begin(); it!=farmMask.end();++it)
   {
      std::string farmKey=(it->first);
      int pIdPosition= farmKey.find("/");
      std::string processId = farmKey.substr(0,pIdPosition);
      std::string farm_add= "/" + (it->second)->parent_address;
      if((strcasecmp(app_name.c_str(),processId.c_str())==0) && (strcasecmp(farm_add.c_str(),farm_address.c_str())==0))
      {
        for(std::set<string>::iterator cmp_iter= ((it->second)->cmpAddress).begin(); cmp_iter!=((it->second)->cmpAddress).end(); ++cmp_iter)
        {
          std::string cmpNode_address=(*cmp_iter);
          unsigned found = cmpNode_address.find_last_of("/");
          std::string url_path=cmpNode_address.substr(0,found);
          std::string cmp_id= cmpNode_address.substr(found+1);
          int numb;
          istringstream ( cmp_id ) >> numb;
          maskVector.at(numb)=OFF;
        }

      }
   }
 return maskVector;   
}
bool DSRIManagerServer::is_mask(std::string farmKey)
{
  std::map<std::string, farmStructure*>::iterator fm_iter= farmMask.find(farmKey);
  if(fm_iter!=farmMask.end())
  {
    farmStructure* fs= fm_iter->second;
    int pIdPosition= farmKey.find("/");
    //std::string processId = farmKey.substr(0,pIdPosition);
    std::string cmp_address = farmKey.substr(pIdPosition);
    std::set<std::string>::iterator cm_iter=fs->cmpAddress.find(cmp_address);
    return ((cm_iter!=((fs->cmpAddress).end())) ? true: false);
  }
  return false;

}
bool DSRIManagerServer::remove_useless_unmask(std::string farmKey)
{
  bool status =false;
  std::map<std::string, farmStructure*>::iterator fm_iter= farmMask.find(farmKey);
  if(fm_iter!=farmMask.end())
  {
    farmStructure* fs= fm_iter->second;
    int pIdPosition= farmKey.find("/");
   // std::string processId = farmKey.substr(0,pIdPosition);
    std::string cmp_address = farmKey.substr(pIdPosition);
    std::set<std::string>::iterator cm_iter=fs->cmpAddress.find(cmp_address);
    if((cm_iter!=((fs->cmpAddress).end())))
    {
      fs->cmpAddress.erase(cmp_address);
      mappingHeuristics* mh = new mappingHeuristics();
      mh->unMaskComponentDevice(farmKey);
      if(fs->cmpAddress.size()==0)
        farmMask.erase(farmKey);
      status=true;
    }
  }
  return status;

}
bool DSRIManagerServer::try_unmaskApp(std::string farmKey, bool& status)
{
  status=true; 
  int changed= false;
  farmStructure* fs= farmMask.at(farmKey);
  int pIdPosition= farmKey.find("/");
  std::string processId = farmKey.substr(0,pIdPosition);
  std::vector<std::string> toBeerased;
  for(std::set<std::string>::iterator cm_iter=fs->cmpAddress.begin(); cm_iter!=fs->cmpAddress.end(); ++cm_iter)
  {
      mappingHeuristics* mh =new mappingHeuristics();
     //std::string cmpNode_address=(*cm_iter);
      //unsigned found = cmpNode_address.find_last_of("/");
      //std::string url_path=cmpNode_address.substr(0,found);
      //std::string cmp_id= cmpNode_address.substr(found+1);
     // int numb;
      //istringstream ( cmp_id ) >> numb;
      //found= url_path.find_last_of("/");
      //std::string parent_path=url_path.substr(0,found);
      //std::string cmp_name = url_path.substr(found+1);

    if(!(mh->try_unmasking(processId+ "/" + (*cm_iter)))) 
    {
     // v.at(numb)=false;
      status=false;
    }
    else 
    {
      toBeerased.push_back(*cm_iter);
      changed=true;
     // v.at(numb)=true;
    }

  }
  for(std::vector<std::string>::iterator e_iter=toBeerased.begin(); e_iter!=toBeerased.end(); ++e_iter)
  {
    fs->cmpAddress.erase(*e_iter);
    --fs->numSuspendedCount;
  }

  return changed;
}
//void DSRIManagerServer::editWorkerStatus(JSONNode& jn , STATUS s)
//{
 // jn.push_back(JSONNode("Status",s));
//}
JSONNode DSRIManagerServer::parse_sensors(const JSONNode& sensors, std::string app_name)
{
	//build instruction goes here;
	if(!(sensors.empty()))
  {
		JSONNode::const_iterator it =sensors.find_nocase("Component_Type");
    std::string cmpType = it->as_string();
    if(strcasecmp(cmpType.c_str(), "farm") == 0 || strcasecmp(cmpType.c_str(), "map") == 0){
    	JSONNode lbActuator(JSON_NODE);
      JSONNode workersActuators(JSON_ARRAY);
      JSONNode workerActuatorsObject(JSON_NODE);
      JSONNode gtActuator(JSON_NODE);
      JSONNode::const_iterator it =sensors.find_nocase("Parallel");
      if(it!=sensors.end())
      {
        workersActuators.set_name("Parallel");
      }
      else
      {
        it =sensors.find_nocase("MISD");
        workersActuators.set_name("MISD");
      }
      JSONNode workersSensors = it->as_array();
      JSONNode workersSensorsObject = workersSensors[0].as_node();

      for(unsigned int i = 0; i < workersSensorsObject.size(); ++i)
      {
        JSONNode workerActuator =DSRIManagerServer::parse_sensors(workersSensorsObject[i].as_node(), app_name);
	  		//STATUS s= DSRIManagerServer::get_masking(workersSensors[i].as_node(), workerActuator);
	  		//maskVector.push_back(s);
	  		//workerActuator.push_back(JSONNode("Status",0));
        workerActuatorsObject.push_back(workerActuator); 
      }
      workersActuators.push_back(workerActuatorsObject);
      JSONNode::const_iterator jnIt= sensors.find_nocase("Node_Address");
      std::vector<STATUS> maskVector = DSRIManagerServer::get_masking(jnIt->as_string(), workersSensorsObject.size(), app_name);
      int i=0;
      for(JSONNode::iterator its=workerActuatorsObject.begin(); its!=workerActuatorsObject.end(); ++its)
      //for(unsigned int i = 0; i < maskVector.size(); ++i)
      {
        //editWorkerStatus(&(workersActuators[i]), maskVector.at(i));
        //JSONNode::iterator its =workersActuators[i];
        JSONNode& jns=*its;
        jns.push_back(JSONNode("Status",maskVector.at(i)));
        ++i;
      }
      lbActuator=DSRIManagerServer::parse_sensors(sensors.at_nocase("1ToNCom").as_node(), app_name);
      // lbActuator.erase(lbActuator.find_nocase("Effective_Ticks")); 
      lbActuator.erase(lbActuator.find_nocase("Service_Time"));
      lbActuator.push_back(get_maskArray(maskVector));
      
      DLT * dlt= new DLT();
      lbActuator.push_back(dlt->get_workload(workerActuatorsObject));

      gtActuator=DSRIManagerServer::parse_sensors(sensors.at_nocase("Nto1Com").as_node(),app_name);
      //  gtActuator.erase(gtActuator.find_nocase("Effective_Ticks"));
      gtActuator.erase(gtActuator.find_nocase("Service_Time"));
      //unsigned long in_come= lbActuator.find_nocase("Ch_Out")->as_int();
      //unsigned long out_come= gtActuator.find_nocase("Total_Number_Of_Tasks")->as_int();
      //double current_arr_rate= lbActuator.find_nocase("Arrival_Rate")->as_float();
      //unsigned long elapsed_time= sensors.find_nocase("Elapsed_Time")->as_float();
      //double arr_rate=DSRIManagerServer::get_arrival_rate(elapsed_time, in_come, out_come, current_arr_rate);
      //lbActuator.at_nocase("Arrival_Rate")=arr_rate;
      JSONNode farmActuator(JSON_NODE);
      farmActuator.push_back(JSONNode("Node_Address",sensors.at_nocase("Node_Address").as_string()));
      farmActuator.push_back(JSONNode("Component_Name", sensors.at_nocase("Component_Name").as_string()));
      farmActuator.push_back(JSONNode("Component_Type",cmpType));
      //farmActuator.push_back(JSONNode("Local_ID",sensors.at_nocase("Local_ID").as_int()));
      unsigned long em_send_task=lbActuator.at_nocase("Ch_Out").as_int();
      unsigned long em_recieve_task=lbActuator.at_nocase("Ch_In").as_int();
      //farmActuator.push_back(JSONNode("Effective_Ticks",DSRIManagerServer::getFarmEffectiveTick(workerActuatorsObject, em_send_task, em_recieve_task)));
      farmActuator.push_back(JSONNode("Service_Time",DSRIManagerServer::getFarmEffectiveTime(workerActuatorsObject, em_send_task, em_recieve_task)));
      //std::cout<<"ERASED::"<<std::endl;
      //std::cout<< workersActuators[0].as_node().write_formatted()<<std::endl;
      lbActuator.erase(lbActuator.find_nocase("Ch_Out"));
      lbActuator.erase(lbActuator.find_nocase("Ch_In"));
      farmActuator.push_back(lbActuator);
      farmActuator.push_back(workersActuators);
      farmActuator.push_back(gtActuator);
      farmActuator.set_name("Composition");
      return farmActuator;
    }
    else if(strcasecmp(cmpType.c_str(), "pipeline") == 0)
    {
  		//JSONNode stagesActuators(JSON_ARRAY);
  		//stagesActuators.set_name("Stages");
      JSONNode pipeActuator(JSON_NODE); 
  	//	ticks maxTick=0;
      double maxServiceTime;
  		//JSONNode stagesSensors= sensors.at_nocase("Stages").as_array();
      for( unsigned int i = 0; i < sensors.size(); ++i )
      {
        if(sensors[i].type()==JSON_NODE)
        {
		      JSONNode stageActuator =DSRIManagerServer::parse_sensors(sensors[i].as_node(),app_name);
		     // ticks eTicks = stageActuator.at_nocase("Effective_Ticks").as_int();
          double serviceTime = stageActuator.at_nocase("Service_Time").as_float();
	      //  stageActuator.erase(stageActuator.find_nocase("Effective_Ticks"));
  	     // if(eTicks>maxTick) maxTick=eTicks;
          if(serviceTime> maxServiceTime) maxServiceTime=serviceTime;
          stageActuator.erase(stageActuator.find_nocase("Service_Time"));
          pipeActuator.push_back(stageActuator);
        }
      }
  		pipeActuator.push_back(JSONNode("Component_Name",sensors.at_nocase("Component_Name").as_string()));
      pipeActuator.push_back(JSONNode("Node_Address",sensors.at_nocase("Node_Address").as_string()));
  		pipeActuator.push_back(JSONNode("Component_Type",cmpType));
  		//pipeActuator.push_back(JSONNode("Local_ID",sensors.at_nocase("Local_ID").as_int()));
  		//pipeActuator.push_back(stagesActuators);
  		//pipeActuator.push_back(JSONNode("Effective_Ticks",maxTick));
      pipeActuator.push_back(JSONNode("Service_Time",maxServiceTime));
      pipeActuator.set_name("Composition");
    	return pipeActuator;
  	}
    else if(strcasecmp(cmpType.c_str(), "sequential") == 0)
    {
  		JSONNode compActuator(JSON_NODE);
      compActuator.push_back(JSONNode("Node_Address",sensors.at_nocase("Node_Address").as_string()));
  		compActuator.push_back(JSONNode("Component_Name", sensors.at_nocase("Component_Name").as_string()));
  		compActuator.push_back(JSONNode("Component_Type",cmpType));
  		//compActuator.push_back(JSONNode("Local_ID",sensors.at_nocase("Local_ID").as_int()));
    	//ticks eTicks =DSRIManagerServer::getEffectiveTick(sensors);
    	//compActuator.push_back(JSONNode("Effective_Ticks",eTicks));
      double serviceTime = DSRIManagerServer::getEffectiveTime(sensors);
      compActuator.push_back(JSONNode("Service_Time",serviceTime));
      // must be change

      mappingHeuristics* m_heuristic = new mappingHeuristics();
      std::string cmp_name =app_name+  sensors.at_nocase("Node_Address").as_string();
      int cmpid =m_heuristic->findBestCPUCore(cmp_name, serviceTime, sensors.at_nocase("Assigned_Processor").as_int());
      if(Environment::get_instance()->allocateCPUId(cmpid,cmp_name, serviceTime, sensors.at_nocase("Assigned_Processor").as_int()))
        compActuator.push_back(JSONNode("Assigned_Processor",cmpid));
      else
        compActuator.push_back(JSONNode("Assigned_Processor",-1));
      compActuator.set_name("sequential");
    	return compActuator;
  	}
    else if(strcasecmp(cmpType.c_str(), "spread") == 0 || strcasecmp(cmpType.c_str(), "d-pol") == 0)
    {
      JSONNode lbActuator(JSON_NODE);
      lbActuator.set_name("1ToNCom");
      lbActuator.push_back(JSONNode("Node_Address",sensors.at_nocase("Node_Address").as_string()));
      lbActuator.push_back(JSONNode("Component_Name", sensors.at_nocase("Component_Name").as_string()));
      lbActuator.push_back(JSONNode("Component_Type",cmpType));
    		//ticks eTicks =DSRIManagerServer::getEffectiveLbTick(sensors);
    		//lbActuator.push_back(JSONNode("Effective_Ticks",eTicks));
      double serviceTime = DSRIManagerServer::getEffectiveLbTime(sensors);
        //lbActuator.push_back(JSONNode("Arrival_Rate",sensors.find_nocase("Arrival_Rate")->as_float()));
      lbActuator.push_back(JSONNode("Service_Time",serviceTime));
      mappingHeuristics* m_heuristic = new mappingHeuristics();
      std::string cmp_name =app_name+ sensors.at_nocase("Node_Address").as_string();
      int cmpid =m_heuristic->findBestCPUCore(cmp_name,serviceTime, sensors.at_nocase("Assigned_Processor").as_int());
      if(Environment::get_instance()->allocateCPUId(cmpid, cmp_name,serviceTime, sensors.at_nocase("Assigned_Processor").as_int()))
        lbActuator.push_back(JSONNode("Assigned_Processor",cmpid));
      else
        lbActuator.push_back(JSONNode("Assigned_Processor",-1));
    	lbActuator.push_back(JSONNode("Ch_Out", sensors.at_nocase("Ch_Out").as_int()));
    	lbActuator.push_back(JSONNode("Ch_In",sensors.at_nocase("Ch_In").as_int()));
      JSONNode::const_iterator it =sensors.find_nocase("Filter");
      if(it!=sensors.end())
      {
        JSONNode filter(JSON_NODE);
        filter.set_name("Filter");
        filter.push_back(JSONNode("Component_Name", (it->as_node()).at_nocase("Component_Name").as_string()));
        lbActuator.push_back(filter);
      }
    		return lbActuator;
	  }
    else if(strcasecmp(cmpType.c_str(), "Reduce") == 0 || strcasecmp(cmpType.c_str(), "g-pol") == 0)
    {
			JSONNode gtActuator(JSON_NODE);
			gtActuator.set_name("Nto1Com");
      gtActuator.push_back(JSONNode("Node_Address",sensors.at_nocase("Node_Address").as_string()));
      gtActuator.push_back(JSONNode("Component_Name",sensors.at_nocase("Component_Name").as_string()));
  		gtActuator.push_back(JSONNode("Component_Type",cmpType));
  	 //	ticks eTicks =DSRIManagerServer::getEffectiveTick(sensors);
      double serviceTime = DSRIManagerServer::getEffectiveTime(sensors);
  		//gtActuator.push_back(JSONNode("Effective_Ticks",eTicks));
      gtActuator.push_back(JSONNode("Service_Time",serviceTime));
      gtActuator.push_back(JSONNode("Processed_Tasks", sensors.find_nocase("Processed_Tasks")->as_int()));
      mappingHeuristics* m_heuristic = new mappingHeuristics();
      std::string cmp_name =app_name+ sensors.at_nocase("Node_Address").as_string();
      int cmpid =m_heuristic->findBestCPUCore(cmp_name,serviceTime, sensors.at_nocase("Assigned_Processor").as_int());
      if(Environment::get_instance()->allocateCPUId(cmpid, cmp_name,serviceTime, sensors.at_nocase("Assigned_Processor").as_int()))
        gtActuator.push_back(JSONNode("Assigned_Processor",cmpid));
      else
        gtActuator.push_back(JSONNode("Assigned_Processor",-1));
      JSONNode::const_iterator it =sensors.find_nocase("Filter");
      if(it!=sensors.end())
      {
        JSONNode filter(JSON_NODE);
        filter.set_name("Filter");
        filter.push_back(JSONNode("Component_Name", (it->as_node()).at_nocase("Component_Name").as_string()));
        gtActuator.push_back(filter);
      }
      return gtActuator;
	  }
    else if(strcasecmp(cmpType.c_str(), "Hsequential") == 0)
    {
      JSONNode oclActuator(JSON_NODE);
      oclActuator.push_back(JSONNode("Node_Address",sensors.at_nocase("Node_Address").as_string()));
  		oclActuator.push_back(JSONNode("Component_Name",sensors.at_nocase("Component_Name").as_string()));
  		oclActuator.push_back(JSONNode("Component_Type",cmpType));
  	//	oclActuator.push_back(JSONNode("Local_ID",sensors.at_nocase("Local_ID").as_int()));
  	//	ticks eTicks =DSRIManagerServer::getEffectiveTick(sensors);
      double serviceTime = DSRIManagerServer::getEffectiveTime(sensors);
  	//	oclActuator.push_back(JSONNode("Effective_Ticks",eTicks));
      oclActuator.push_back(JSONNode("Service_Time",serviceTime));
      
      
      mappingHeuristics* m_heuristic = new mappingHeuristics();
      std::string cmp_name =app_name+ sensors.at_nocase("Node_Address").as_string();
      int cmpid =m_heuristic->findBestCPUCore(cmp_name,serviceTime, sensors.at_nocase("Assigned_Processor").as_int());
      if(Environment::get_instance()->allocateCPUId(cmpid,cmp_name,serviceTime, sensors.at_nocase("Assigned_Processor").as_int()))
        oclActuator.push_back(JSONNode("Assigned_Processor",cmpid));
      else
        oclActuator.push_back(JSONNode("Assigned_Processor",-1));

      int cl_id;
      int e_val = sensors.at_nocase("End_Received").as_int();
      if(e_val==1) cl_id=0;

      //int cl_id =m_heuristic->findBestGPUDevice(cmp_name);    
      else cl_id =m_heuristic->get_gpu_device(cmp_name);      
      //std::cout<<"This is the selected Device:" << cl_id<< std::endl; 
      //std::cout<< "this is my URL"<<sensors.at_nocase("Node_Address").as_string()<< std::endl;
      //if(
        //Environment::get_instance()->allocateGPUId(cl_id,cmp_name);
        //)
      //{
        oclActuator.push_back(JSONNode("Device_Number",cl_id));
        oclActuator.set_name("Hsequential");
      //}
     // else
      //{
      
       // exit(EXIT_FAILURE);
      //}
  		return oclActuator;
	  }
	}
};


JSONNode DSRIManagerServer::find_buttleneck_node(const JSONNode& btnk_path)
{
    if(!(btnk_path.empty()))
    {
        JSONNode::const_iterator it =btnk_path.find_nocase("Component_Type");
        std::string cmpType = it->as_string();
        if(strcasecmp(cmpType.c_str(), "farm") == 0 || strcasecmp(cmpType.c_str(), "map") == 0 || strcasecmp(cmpType.c_str(), "pipeline") == 0)
        {
            return DSRIManagerServer::find_buttleneck_node((btnk_path.at_nocase("Buttleneck")).as_node());
        }
        else
        {
            return btnk_path;
        }
    }

};
// this function also updated all the new sensor information over the environment
JSONNode DSRIManagerServer::find_buttleneck_path(std::string app_name, const JSONNode& sensors,double elapsed_time)
{
  //build instruction goes here;
  if(!(sensors.empty()))
  {
    JSONNode::const_iterator it =sensors.find_nocase("Component_Type");
      std::string cmpType = it->as_string();
      JSONNode workersSensors(JSON_ARRAY);
      if(strcasecmp(cmpType.c_str(), "farm") == 0 || strcasecmp(cmpType.c_str(), "map") == 0 )
      {
        JSONNode lbNd(JSON_NODE);
        JSONNode gtNd(JSON_NODE);
        JSONNode::const_iterator it =sensors.find_nocase("Parallel");
        if(it!=sensors.end())
        {
          workersSensors.set_name("Parallel");
        }
        else
        {
          it =sensors.find_nocase("MISD");
          workersSensors.set_name("MISD");
        }


        //JSONNode::const_iterator it =sensors.find_nocase("Workers");
        workersSensors = it->as_array();
        JSONNode workersSensorsObject = workersSensors[0].as_node();
        JSONNode btlWorkerNd =DSRIManagerServer::find_buttleneck_path(app_name ,workersSensorsObject[0].as_node(), elapsed_time);
        double maxRate =-1;// btlWorkerNd.at_nocase("Efficiency_Rate").as_float(); 
        for(unsigned int i = 0; i < workersSensorsObject.size(); ++i)
        {
          JSONNode workerNd =DSRIManagerServer::find_buttleneck_path(app_name,workersSensorsObject[i].as_node(), elapsed_time);
          double currRate = workerNd.at_nocase("Efficiency_Rate").as_float(); 
          if( maxRate==-1 || currRate<maxRate)
          {
            btlWorkerNd=workerNd;
            maxRate=currRate;
          }
        }

        JSONNode farmBtnNd(JSON_NODE);
        farmBtnNd.push_back(JSONNode("Component_Type",cmpType));
        farmBtnNd.push_back(JSONNode("Node_Address",sensors.at_nocase("Node_Address").as_string()));
        btlWorkerNd.set_name("Buttleneck");
        farmBtnNd.push_back(btlWorkerNd);
        
        lbNd=sensors.at_nocase("1ToNCom").as_node();
        JSONNode::const_iterator farm_input_it =lbNd.find_nocase("Ch_Out");
        gtNd=sensors.at_nocase("Nto1Com").as_node();
        JSONNode::const_iterator farm_output_it =gtNd.find_nocase("Processed_Tasks");
        double farmRate = ((farm_output_it->as_int())*1.0)/(farm_input_it->as_int());

        //   farmBtnNd.push_back(JSONNode("Efficiency_Rate",farmRate));
        farmBtnNd.push_back(JSONNode("Efficiency_Rate",maxRate)); 

        return farmBtnNd;

      }
      else if(strcasecmp(cmpType.c_str(), "pipeline") == 0)
      {
          
        //JSONNode stagesSensors= sensors.at_nocase("Stages").as_array();
        JSONNode btnStage(JSON_NODE);// =DSRIManagerServer::find_buttleneck_path(app_name,stagesSensors[0].as_node(), elapsed_time);
        double maxServiceRate  =-1;//btnStage.at_nocase("Efficiency_Rate").as_float(); 
        for( unsigned int i = 0; i < sensors.size(); ++i )
        {
           if(sensors[i].type()==JSON_NODE)
          {
            JSONNode currStage =DSRIManagerServer::find_buttleneck_path(app_name,sensors[i].as_node(),elapsed_time);
            double serviceRate =currStage.at_nocase("Efficiency_Rate").as_float();
            
            if(maxServiceRate==-1 || serviceRate< maxServiceRate)
            { 
              maxServiceRate=serviceRate;
              btnStage=currStage;
            }
          }

        }
        JSONNode pipeBtNd(JSON_NODE); 
        pipeBtNd.push_back(JSONNode("Component_Type",cmpType));
        pipeBtNd.push_back(JSONNode("Node_Address",sensors.at_nocase("Node_Address").as_string()));
        btnStage.set_name("Buttleneck");
        pipeBtNd.push_back(btnStage);
        pipeBtNd.push_back(JSONNode("Efficiency_Rate",maxServiceRate));
        return pipeBtNd;
      }
      else
      {
        JSONNode cmpBtnNd(JSON_NODE);
        cmpBtnNd.push_back(JSONNode("Component_Type",cmpType));

        std::string cmp_name =app_name+ sensors.at_nocase("Node_Address").as_string();
        //double serviceTime;
        // ((strcasecmp(cmpType.c_str(), "FF::LOADBALANCER") == 0))?
        //  serviceTime = DSRIManagerServer::getEffectiveLbTime(sensors):
        // double serviceTime=DSRIManagerServer::getEffectiveTime(sensors);
        double serviceTime = DSRIManagerServer::get_last_serviceTime(sensors);
        //if 
        //{
           // std::cout<<"updated"<<std::endl;
        //}
        
        
        cmpBtnNd.push_back(JSONNode("Node_Address",sensors.at_nocase("Node_Address").as_string()));
        JSONNode::const_iterator cInput_iter =sensors.find_nocase("Queue_Input");
        JSONNode::const_iterator cOutput_iter =sensors.find_nocase("Queue_Output");
        JSONNode::const_iterator cServiceTime_iter =sensors.find_nocase("Total_Component_Active_Time");
        cmpBtnNd.push_back(JSONNode("Active_Time",sensors.at_nocase("Total_Component_Active_Time").as_string()));
        cmpBtnNd.push_back(JSONNode("Elapsed_Time",sensors.at_nocase("Elapsed_Time").as_string()));
        cmpBtnNd.push_back(JSONNode("Service_Time",serviceTime));
       // cmpBtnNd.push_back(JSONNode("total_time",sensors.at_nocase("Total_SVC_Time").as_string()));

        JSONNode::const_iterator end_it = sensors.find_nocase("End_Received");
        //double btnRate;
        if(end_it!=sensors.end() && ((end_it->as_int())==1))
        {
          
         // btnRate=USELESS_NODE;
         
          if(strcasecmp(cmpType.c_str(), "Hsequential") == 0)
          {
            Environment::get_instance()->deallocateGPUComponent(cmp_name);
            remove_useless_unmask(cmp_name);
          }
         
        //  cmpBtnNd.push_back(JSONNode("Queue_Output",0));
         // cmpBtnNd.push_back(JSONNode("Queue_Input",0));
         // cmpBtnNd.push_back(JSONNode("Queue_Throughput",1));
         // cmpBtnNd.push_back(JSONNode("Component_Utilisation",0));
          registered_apps[app_name]->has_useless=true;
          (registered_apps[app_name]->useles_node_address).insert((sensors.at_nocase("Node_Address").as_string()));
        }
       // else
        //{
          (Environment::get_instance()->updateCPUComponetServiceTime(cmp_name,serviceTime)) ;
        //}
          int inpt_val= cInput_iter->as_int();
          double queue_throughput=0; // the queue is not added here
          /*if(inpt_val!=0) 
            queue_throughput=((cOutput_iter->as_int())*1.0)/(inpt_val);
          if(queue_throughput<0 ) queue_throughput=1;*/
          if(elapsed_time==0) elapsed_time=-1;
          //if(serviceTime==0) {DLT_STATUS=true;}
          double component_utilisation=((serviceTime)/(elapsed_time));//(( (cServiceTime_iter->as_float())/(elapsed_time)));
          std::cout<<"************name*************"<<(sensors.at_nocase("Node_Address").as_string())<<std::endl;
          std::cout<<"******************cmpu************"<< component_utilisation <<std::endl;
          std::cout <<"******************QT************"<< queue_throughput<< std::endl;
          std::cout <<"******************servicetime************"<< serviceTime<< std::endl;
          
          if(component_utilisation<0) component_utilisation=0;
          cmpBtnNd.push_back(JSONNode("Queue_Output",cOutput_iter->as_int()));
          cmpBtnNd.push_back(JSONNode("Queue_Input",cInput_iter->as_int()));
          cmpBtnNd.push_back(JSONNode("Queue_Throughput",queue_throughput));
          cmpBtnNd.push_back(JSONNode("Component_Utilisation",component_utilisation));
          double btnRate =((end_it!=sensors.end() && ((end_it->as_int())==1))? (USELESS_NODE):(queue_throughput + (1.0-component_utilisation)));
          std::cout <<"******************btnRate************"<< btnRate<< std::endl;
       // }
        // this is because we only tune oclNodes. there for it shows amongs oclNodes buttleneck not the real buttlenecks
        (strcasecmp(cmpType.c_str(), "Hsequential") == 0)?
            cmpBtnNd.push_back(JSONNode("Efficiency_Rate",btnRate)):
            cmpBtnNd.push_back(JSONNode("Efficiency_Rate",NOT_OCLNODE));

        //        cmpBtnNd.push_back(JSONNode("Efficiency_Rate",btnRate));
        return cmpBtnNd;
      }
  }
}


JSONNode DSRIManagerServer::build_Architectural_info(nodeInfo* node_info)
{
  JSONNode architecutal_info(JSON_NODE);
  architecutal_info.push_back(JSONNode("Host_Name", node_info->hostName));
  architecutal_info.push_back(JSONNode("Interface_Name_and_Family",node_info->interfaceNameFamily));
  architecutal_info.push_back(JSONNode("Interface_Address", node_info->interfaceAddress));
  architecutal_info.push_back(JSONNode("Number_of_Cores", node_info->numCPU));
  architecutal_info.push_back(JSONNode("Memory_Size", node_info->totalRam));
  architecutal_info.push_back(JSONNode("OpenCL_Enable", node_info->OpenCL_enable));  
  JSONNode deviceArr(JSON_ARRAY);
  deviceArr.set_name("Devices");
  if(node_info->OpenCL_enable){
    for(int i=0; i<node_info->DevicesInfo.size();i++){
      JSONNode device_info(JSON_NODE);
      device_info.push_back(JSONNode("Device_Platform_Number",node_info->DevicesInfo.at(i)->devicePlatformNumber));
      device_info.push_back(JSONNode("Device_Platform_Name",node_info->DevicesInfo.at(i)->devicePlatformName));
      device_info.push_back(JSONNode("Device_Platform_Vendor",node_info->DevicesInfo.at(i)->devicePlatformVendor));
      device_info.push_back(JSONNode("Device_Number",node_info->DevicesInfo.at(i)->deviceNumber));
      device_info.push_back(JSONNode("Device_Type",node_info->DevicesInfo.at(i)->deviceType));
      device_info.push_back(JSONNode("Device_Vendor",node_info->DevicesInfo.at(i)->deviceVendor));
      device_info.push_back(JSONNode("Device_Global_Memory_Size",node_info->DevicesInfo.at(i)->deviceGlobalMemorySize));
      device_info.push_back(JSONNode("Number_of_SM_Per_Device",node_info->DevicesInfo.at(i)->SMNumbers));
      deviceArr.push_back(device_info);
    }
  }
  architecutal_info.push_back(deviceArr);
  return architecutal_info;

};

std::string DSRIManagerServer::improve_btn_path(std::string app_name)
{ 
       
    //Environment::get_instance()->releaseResources(app_name);

    JSONNode sensors(JSON_NULL);
    sensors= libjson::parse(registered_apps[app_name]->sensor);  

    JSONNode actuators= DSRIManagerServer::parse_DLT_sensor(sensors, app_name);
    //actuators.erase(actuators.find_nocase("Effective_Ticks"));
    actuators.erase(actuators.find_nocase("Service_Time"));
   

    return actuators.write_formatted();
};

template<typename T> std::string DSRIManagerServer::toString(T val){
    stringstream ss_reader;
    std::string s_reader;
    ss_reader<<val;
    ss_reader>>s_reader;
    return s_reader;
}

double DSRIManagerServer::get_last_serviceTime(const JSONNode& sensors)
{
    double serviceTime=0.0;
    JSONNode::const_iterator it =sensors.find_nocase("Component_Last_Processing_Time");
    if(it!=sensors.end())
        {        
            serviceTime = it->as_float();
        }
    return serviceTime;
}



JSONNode DSRIManagerServer::parse_DLT_sensor(const JSONNode& sensors, std::string app_name)
{
    //build instruction goes here;
    if(!(sensors.empty()))
    {
        JSONNode::const_iterator it =sensors.find_nocase("Component_Type");
        std::string cmpType = it->as_string();
        if(strcasecmp(cmpType.c_str(), "farm") == 0 || strcasecmp(cmpType.c_str(), "map") == 0)
        {
            JSONNode lbActuator(JSON_NODE);
            JSONNode workersActuators(JSON_ARRAY);
            JSONNode workersActuatorsObject(JSON_NODE);

            //workersActuators.set_name("Workers");
            JSONNode gtActuator(JSON_NODE);
            //std::vector<STATUS> maskVector;
           // JSONNode::const_iterator it =sensors.find_nocase("Workers");

            JSONNode::const_iterator it =sensors.find_nocase("Parallel");
            if(it!=sensors.end())
            {
              workersActuators.set_name("Parallel");
            }
            else
            {
              it =sensors.find_nocase("MISD");
              workersActuators.set_name("MISD");
            }
            JSONNode workersSensors = it->as_array();
            JSONNode workersSensorsObject = workersSensors[0].as_node();
            for(unsigned int i = 0; i < workersSensorsObject.size(); ++i){
              JSONNode workerActuator =DSRIManagerServer::parse_DLT_sensor(workersSensorsObject[i].as_node(), app_name);
              workersActuatorsObject.push_back(workerActuator); 
            }
            JSONNode::const_iterator jnIt= sensors.find_nocase("Node_Address");
            std::vector<STATUS> maskVector = DSRIManagerServer::get_masking(jnIt->as_string(), workersSensorsObject.size(), app_name);
            int i=0;
            for(JSONNode::iterator its=workersActuatorsObject.begin(); its!=workersActuatorsObject.end(); ++its)
            //for(unsigned int i = 0; i < maskVector.size(); ++i)
            {
              //editWorkerStatus(&(workersActuators[i]), maskVector.at(i));
              //JSONNode::iterator its =workersActuators[i];
              JSONNode& jns=*its;
              jns.push_back(JSONNode("Status",maskVector.at(i)));
              ++i;
            }

            workersActuators.push_back(workersActuatorsObject);
    
            lbActuator=DSRIManagerServer::parse_DLT_sensor(sensors.at_nocase("1ToNCom").as_node(),app_name);
           // lbActuator.erase(lbActuator.find_nocase("Effective_Ticks"));
            lbActuator.erase(lbActuator.find_nocase("Service_Time"));
            lbActuator.push_back(get_maskArray(maskVector));
            
            DLT * dlt= new DLT();
            lbActuator.push_back(dlt->get_workload(workersActuatorsObject));
            //JSONNode workersDLT(JSON_ARRAY);
            //workersDLT=dlt->get_workload(workersActuators).as_array();
            //lbActuator.push_back(workersDLT);
            //for(int i=0; workersDLT.size(); ++i)
            //  {
            //       dlt_distributed.push_back((toString(i) + "," + (workersDLT[i].as_string())));
            //   }
            //JSONNode workersDLT = dlt->get_workload(workersActuators);
            if(strcasecmp((registered_apps.at(app_name)->application_file_id).c_str(), "_users_mehdi_Thesis_fastflow-2.0.2.11-VIP_examples_simple-convolution_Lib_version_build_nested.json")==0)
            {
              JSONNode::const_iterator arr_it =lbActuator.find_nocase("Workload");
              if(arr_it->type()==JSON_ARRAY)
              {
                  JSONNode jn_dlt = arr_it->as_array();
                //  std::cout<< arr_it->write_formatted(); 
              //lbActuator.push_back(workersDLT);//dlt->get_workload(workersActuators));
                  for(int i=0; i<jn_dlt.size(); ++i)
                  {
                      //std::string str 
                      double j = ( jn_dlt[i].as_float());
                      std::string s;//(i + ",");
                      s=toString(i) + "," + toString(j) + "\n";
                      dlt_distributed.push_back(s);
                  }
              }
            }
            gtActuator=DSRIManagerServer::parse_DLT_sensor(sensors.at_nocase("Nto1Com").as_node(),app_name);
         //   gtActuator.erase(gtActuator.find_nocase("Effective_Ticks"));
            gtActuator.erase(gtActuator.find_nocase("Service_Time"));
            
            //unsigned long in_come= lbActuator.find_nocase("Total_Number_Of_Send_Tasks")->as_int();
            //unsigned long out_come= gtActuator.find_nocase("Total_Number_Of_Tasks")->as_int();
            //double current_arr_rate= lbActuator.find_nocase("Arrival_Rate")->as_float();
            //unsigned long elapsed_time= sensors.find_nocase("Elapsed_Time")->as_float();
            //double arr_rate=DSRIManagerServer::get_arrival_rate(elapsed_time, in_come, out_come, current_arr_rate);
            
            //lbActuator.at_nocase("Arrival_Rate")=arr_rate;
            JSONNode farmActuator(JSON_NODE);
            
            farmActuator.push_back(JSONNode("Node_Address",sensors.at_nocase("Node_Address").as_string()));
            farmActuator.push_back(JSONNode("Component_Type",cmpType));
            
            unsigned long em_send_task=lbActuator.at_nocase("Ch_Out").as_int();
            unsigned long em_recieve_task=lbActuator.at_nocase("Ch_In").as_int();
            
            //farmActuator.push_back(JSONNode("Effective_Ticks",DSRIManagerServer::getFarmEffectiveTick(workersActuators, em_send_task, em_recieve_task)));
            farmActuator.push_back(JSONNode("Service_Time",DSRIManagerServer::getFarmEffectiveTime(workersActuators, em_send_task, em_recieve_task)));
            lbActuator.erase(lbActuator.find_nocase("Ch_Out"));
            lbActuator.erase(lbActuator.find_nocase("Ch_In"));
            farmActuator.push_back(lbActuator);
            farmActuator.push_back(workersActuators);
            farmActuator.push_back(gtActuator);
            farmActuator.set_name("Composition");
            
            return farmActuator;
        }
        else if(strcasecmp(cmpType.c_str(), "pipeline") == 0)
        {
            //JSONNode stagesActuators(JSON_ARRAY);
            //stagesActuators.set_name("Stages");
           JSONNode pipeActuator(JSON_NODE);   
          //  ticks maxTick=0;
            double maxServiceTime;
            //JSONNode stagesSensors= sensors.at_nocase("Stages").as_array();
            
            for( unsigned int i = 0; i < sensors.size(); ++i )
            {
              if(sensors[i].type()==JSON_NODE)
              {

                JSONNode stageActuator =DSRIManagerServer::parse_DLT_sensor(sensors[i].as_node(),app_name);
                //ticks eTicks = stageActuator.at_nocase("Effective_Ticks").as_int();
                double serviceTime = stageActuator.at_nocase("Service_Time").as_float();
                //stageActuator.erase(stageActuator.find_nocase("Effective_Ticks"));
                //if(eTicks>maxTick) maxTick=eTicks;
                if(serviceTime> maxServiceTime) maxServiceTime=serviceTime;
                stageActuator.erase(stageActuator.find_nocase("Service_Time"));
                pipeActuator.push_back(stageActuator);
              }  
            }
            
            pipeActuator.push_back(JSONNode("Node_Address",sensors.at_nocase("Node_Address").as_string()));
            pipeActuator.push_back(JSONNode("Component_Type",cmpType));
            //pipeActuator.push_back(stagesActuators);
            //pipeActuator.push_back(JSONNode("Effective_Ticks",maxTick));
            pipeActuator.push_back(JSONNode("Service_Time",maxServiceTime));

            pipeActuator.set_name("Composition");
            
            return pipeActuator;
        }
        else if(strcasecmp(cmpType.c_str(), "sequential") == 0)
        {
            JSONNode compActuator(JSON_NODE);
            compActuator.push_back(JSONNode("Node_Address",sensors.at_nocase("Node_Address").as_string()));
            compActuator.push_back(JSONNode("Component_Type",cmpType));
          //  ticks eTicks =DSRIManagerServer::getEffectiveTick(sensors);
           // compActuator.push_back(JSONNode("Effective_Ticks",eTicks));
            double serviceTime;
            (sMode==AD_HOC)?
              serviceTime = DSRIManagerServer::get_last_serviceTime(sensors):
              serviceTime = DSRIManagerServer::getEffectiveTime(sensors); 
            
            compActuator.push_back(JSONNode("Service_Time",serviceTime));
            compActuator.set_name("sequential");
            
            return compActuator;
        }
        else if(strcasecmp(cmpType.c_str(), "Spread") == 0 || strcasecmp(cmpType.c_str(), "d-pol") == 0)
        {
            JSONNode lbActuator(JSON_NODE);
            lbActuator.set_name("1ToNCom");
            lbActuator.push_back(JSONNode("Node_Address",sensors.at_nocase("Node_Address").as_string()));
            lbActuator.push_back(JSONNode("Component_Type",cmpType));
          //  ticks eTicks =DSRIManagerServer::getEffectiveLbTick(sensors);
           // lbActuator.push_back(JSONNode("Effective_Ticks",eTicks));
            double serviceTime;
            (sMode==AD_HOC)?
              serviceTime = DSRIManagerServer::get_last_serviceTime(sensors):
              serviceTime = DSRIManagerServer::getEffectiveTime(sensors); 

            //double serviceTime = DSRIManagerServer::getEffectiveLbTime(sensors);
            //double serviceTime = DSRIManagerServer::get_last_serviceTime(sensors);
        //    lbActuator.push_back(JSONNode("Arrival_Rate",sensors.find_nocase("Arrival_Rate")->as_float()));
            lbActuator.push_back(JSONNode("Service_Time",serviceTime));
            lbActuator.push_back(JSONNode("Ch_Out", sensors.at_nocase("Ch_Out").as_int()));
            lbActuator.push_back(JSONNode("Ch_In",sensors.at_nocase("Ch_In").as_int()));
            
            return lbActuator;
        }
        else if(strcasecmp(cmpType.c_str(), "Reduce") == 0 || strcasecmp(cmpType.c_str(), "g-pol") == 0)
        {
            JSONNode gtActuator(JSON_NODE);
            gtActuator.set_name("Nto1Com");
            gtActuator.push_back(JSONNode("Node_Address",sensors.at_nocase("Node_Address").as_string()));
            gtActuator.push_back(JSONNode("Component_Type",cmpType));
           // ticks eTicks =DSRIManagerServer::getEffectiveTick(sensors);
            
            double serviceTime;
            (sMode==AD_HOC)?
              serviceTime = DSRIManagerServer::get_last_serviceTime(sensors):
              serviceTime = DSRIManagerServer::getEffectiveTime(sensors); 
            // double serviceTime = DSRIManagerServer::getEffectiveTime(sensors);
            //double serviceTime = DSRIManagerServer::get_last_serviceTime(sensors);
          //  gtActuator.push_back(JSONNode("Effective_Ticks",eTicks));
            gtActuator.push_back(JSONNode("Service_Time",serviceTime));
            gtActuator.push_back(JSONNode("Processed_Tasks", sensors.find_nocase("Processed_Tasks")->as_int()));
            
            return gtActuator;
        }
        else if(strcasecmp(cmpType.c_str(), "Hsequential") == 0)
        {
            JSONNode oclActuator(JSON_NODE);
            oclActuator.push_back(JSONNode("Node_Address",sensors.at_nocase("Node_Address").as_string()));
            oclActuator.push_back(JSONNode("Component_Type",cmpType));
           // ticks eTicks =DSRIManagerServer::getEffectiveTick(sensors);
            double serviceTime;
            (sMode==AD_HOC)?
              serviceTime = DSRIManagerServer::get_last_serviceTime(sensors):
              serviceTime = DSRIManagerServer::getEffectiveTime(sensors); 

            //double serviceTime = DSRIManagerServer::getEffectiveTime(sensors);
            //double serviceTime = DSRIManagerServer::get_last_serviceTime(sensors);

           /* test for editActuator
            set<string>::iterator it= currently_selected_apps.begin();
            JSONNode jn1(JSON_NODE);
            jn1= libjson::parse(registered_apps[(*it)]->btn_node);
            std::string myName = jn1.at("Name").as_string();
            std::string my_add =registered_apps[(*it)]->btn_path;
            JSONNode jn = libjson::parse(registered_apps[(*it)]->btn_path);
            editActuator(jn, myName, "Efficiency_Rate", "0");
            */
            
            //std::cout<< "MYOCL VALUE:" << serviceTime<<std::endl;
          //  oclActuator.push_back(JSONNode("Effective_Ticks",eTicks));
            oclActuator.push_back(JSONNode("Service_Time",serviceTime));
            oclActuator.set_name("Hsequential");
            return oclActuator;

        }
    }
};


//};
