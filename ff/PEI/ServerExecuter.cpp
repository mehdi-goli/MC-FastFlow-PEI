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

//
//  This is a DSRI manager per node it is a server for each node containing the information regarding that node.
//  All of the PEI enable applications will register here and connected to the system
//  This node itself will connect to the DSRI Manager Controller which control different DSRI manager nodes.
//  Binds REP socket to tcp://*:5555
//

#include <zmq.hpp>
#include <string>
#include <iostream>
#include <unistd.h>
#include <zhelpers.hpp>
#include <ff/PEI/DSRIManagerServer.hpp>
#include <ff/PEI/ApplicationDatabase.hpp>
 #include <iostream>     // std::cout
#include <fstream>      // std::fstream
using namespace std;
//zmq::context_t context_server (1);
//zmq::socket_t socket_server (context_server, ZMQ_REP);

zmq::context_t context(1);
zmq::socket_t socket_server (context, ZMQ_ROUTER);
//#define PERIOD 5
#define POLL_WAIT 1000


void prepareZMQconnection()
{
    std::cout << " Providing Communication channel for clients" << std::endl;
   
    //char result[ PATH_MAX ];
   // size_t count = readlink( "/proc/self/exe", result, PATH_MAX );
   // std::string exe_path= std::string( result, (count > 0) ? count : 0 );
   // unsigned found = exe_path.find_last_of("/");
    std::string path = std::string("/tmp"); //exe_path.substr(0, found);
    std::string ZMQFolder= std::string("ZMQpath/");  
    struct stat st = {0};
    
    if (stat((path +"/" +ZMQFolder).c_str(), &st) == -1) 
    {
      mkdir((path +"/" +ZMQFolder).c_str(), 0700);
    }

   if (!(std::ifstream((path + "/" +ZMQFolder + "ZMQWorkspace.ipc").c_str())))
    {
      //std::cout << "File already exists" << std::endl;
      std::ofstream file((path + "/" +ZMQFolder + "ZMQWorkspace.ipc").c_str());
      file.close();
    }//else {
		//std::ofstream file((path + "/" +ZMQFolder + "ZMQWorkspace.ipc").c_str());
		//file.close();
	//}

    std::string zqp="ipc://" + path + "/" +ZMQFolder + "ZMQWorkspace.ipc";
   // socket_server.bind ("tcp://*:5555");
    socket_server.setsockopt( ZMQ_IDENTITY, "DSRIServer", 10);
    socket_server.bind(zqp.c_str());
}

struct cMessage{
    cMessage(){}
    std::string content;
    std::string senderId;
    void reciveMessage(){
        senderId= s_recv (socket_server);
        content = s_recv (socket_server);

        //std::cout<<"Content:"<<content<<std::endl;
    }
    void sendMessage(){
        s_sendmore (socket_server, (senderId).data());
        s_send (socket_server, (content).data());
    }
};


void calculate_age(std::string pid){
    //for(set<string>::iterator c_iter= currently_selected_apps.begin(); c_iter !=currently_selected_apps.end(); ++c_iter)
    //{
        
        if(registered_apps.at(pid)->activation_status)
        {
            ++(registered_apps.at(pid)->priority);
            //std::cout<< "NEW PRIORITY: ___+++_)_+_+)_+ for " <<*c_iter <<":"<<(registered_apps.at(*c_iter)->priority)<<std::endl;
        }
    //}
}

void calculate_age(){
    for(set<string>::iterator c_iter= currently_selected_apps.begin(); c_iter !=currently_selected_apps.end(); ++c_iter)
    {
        
        if(registered_apps.at(*c_iter)->activation_status)
        {
            ++(registered_apps.at(*c_iter)->priority);
            //std::cout<< "NEW PRIORITY: ___+++_)_+_+)_+ for " <<*c_iter <<":"<<(registered_apps.at(*c_iter)->priority)<<std::endl;
        }
    }
}

void farmMasking(DSRIManagerServer* dsriManagerServer)
{
    for(map<string, farmStructure*>::iterator c_iter= farmMask.begin(); c_iter !=farmMask.end(); ++c_iter)
    {
        int pIdPosition= (c_iter->first).find("/");
        std::string processId = (c_iter->first).substr(0,pIdPosition);
        cMessage* msg_rep=new cMessage;
        msg_rep->senderId=processId;
        std::cout << "+++++++++++++masking:++++++++++++++++++++++"<< msg_rep->senderId<<std::endl;
        msg_rep->content=dsriManagerServer->improve_btn_path(processId); //dsriManagerServer->get_masking(c_iter->first); // must be implemented
        msg_rep->sendMessage();   
    }

}
void farmUnMasking(DSRIManagerServer* dsriManagerServer)
{
    set<std::string> keys;
    for(map<string, farmStructure*>::iterator it = farmMask.begin(); it != farmMask.end(); ++it) 
    {
        keys.insert(it->first);
    }
    std::vector<std::string> farm_apps =prioritySort_farm(keys);
    std::vector<std::string> toBeRemoved;
    for(int i=0; i<farm_apps.size(); ++i)
    {
        bool status=false;
        if(dsriManagerServer->try_unmaskApp(farm_apps.at(i), status)){
        int pIdPosition= (farm_apps.at(i)).find("/");
        std::string processId = (farm_apps.at(i)).substr(0,pIdPosition);
        cMessage* msg_rep=new cMessage;
        msg_rep->senderId=processId;
        std::cout << "+++++++++++++ unmasking: ++++++++++++++++++++++"<< msg_rep->senderId<<std::endl;
        msg_rep->content=dsriManagerServer->improve_btn_path(processId);//dsriManagerServer->get_masking(farm_apps.at(i)); // must be implemented
        //std::cout<< "!!!!!!!!!!!!!! THe Actuator!!!!!!!!!"<<msg_rep->content<<std::endl;
        msg_rep->sendMessage();
        }
        if(status)
        {
            toBeRemoved.push_back(farm_apps.at(i));
        }
    }
    for(int i=0; i<toBeRemoved.size(); ++i)
    {
        farmMask.erase(toBeRemoved.at(i));
    }

}

bool suspend_applications(std::vector<std::string> old_supended, DSRIManagerServer* dsriManagerServer)
{
    std::set<std::string> suspended(old_supended.begin(), old_supended.end());

    for(std::set<std::string>::iterator s_iter= suspended_apps.begin(); s_iter!=suspended_apps.end(); ++s_iter)
    {
        std::set<std::string>::iterator c_iter =suspended.find(*s_iter);
        if(c_iter==suspended.end())
        {
            cMessage* msg_rep=new cMessage;
            msg_rep->senderId=(*s_iter);
             std::cout << "+++++++++++++suspendig: ++++++++++++++++++++++"<< msg_rep->senderId<<std::endl;
            msg_rep->content=dsriManagerServer->get_action("Suspend");
            msg_rep->sendMessage();   
        }
    }
    return true;
}
bool modified_application_notifier()
{
    
    for(std::set<std::string>::iterator it =modified_actuators_apps.begin(); it!=modified_actuators_apps.end(); ++it)
    {
        cMessage* msg_rep=new cMessage;
        msg_rep->senderId=*it;
        std::cout<< "this is the left ID:::::::"<<msg_rep->senderId<< std::endl;
        msg_rep->content=registered_apps.at(msg_rep->senderId)->actuator;
        msg_rep->sendMessage();

    }
    
    modified_actuators_apps.clear();
}

bool resume_suspended_apps(DSRIManagerServer * dsriManagerServer)
{
    std::vector<std::string> sus_apps=prioritySort(suspended_apps);
     std::set<std::string> resumed;
    for(std::vector<std::string>::iterator it =sus_apps.begin(); it!=sus_apps.end(); ++it)
    {
        registered_apps.at(*it)->activation_status=true;
        std::string actuator = dsriManagerServer->build_actuators(*it);
        modified_application_notifier();
        if(registered_apps.at(*it)->activation_status)
        {
            registered_apps[*it]->actuator=actuator;
            // first set the correct actuator
            cMessage* msg_rep=new cMessage;
            msg_rep->senderId=(*it);
            std::cout << "+++++++++++++resuming: ++++++++++++++++++++++"<< msg_rep->senderId<<std::endl;
            msg_rep->content=actuator;
            std::cout << "+++++++++++++actuator file:++++++++++++++++++++++"<< actuator<<std::endl;
            msg_rep->sendMessage();
            // then resume
            std::set<std::string>::iterator iter = resumed.find(msg_rep->senderId);
            if(iter==resumed.end())
                resumed.insert(msg_rep->senderId);

            msg_rep->content=dsriManagerServer->get_action("Resume");
            msg_rep->sendMessage();

        }


    for(std::set<std::string>::iterator iter =resumed.begin(); iter!=resumed.end(); ++iter)
    {
        suspended_apps.erase(*iter);
    }
    
    }
    farmUnMasking(dsriManagerServer);
    // now send message to all new deactivated apps and tell them to sleep
    suspend_applications(sus_apps, dsriManagerServer);
    farmMasking(dsriManagerServer);
    return true;

}

bool register_application(cMessage* msg_req, DSRIManagerServer* dsriManagerServer)
{
    std::vector<std::string> sus_apps=prioritySort(suspended_apps);
    registered_apps[msg_req->senderId]=  new app_info;
    dsriManagerServer->load_sensor_file(msg_req->senderId,msg_req->content);
    //registered_apps[msg_req->senderId]->sensor= msg_req->content;
    //if(pr==FIX)
     //   registered_apps[msg_req->senderId]->priority=static_cast<int>(get_current_time());
    //else
      //  registered_apps[msg_req->senderId]->priority=0;
    registered_apps[msg_req->senderId]->actuator =dsriManagerServer->build_actuators(msg_req->senderId);
    //update all the changed application
    modified_application_notifier();
     // now send message to all deactivated apps and tell them to sleep
    sus_apps.clear();
    suspend_applications(sus_apps, dsriManagerServer);
    //sensd the registered value to the application
    cMessage* msg_rep=new cMessage;
    msg_rep->content=registered_apps[msg_req->senderId]->actuator;
    msg_rep->senderId= msg_req->senderId;
    msg_rep->sendMessage();
    farmMasking(dsriManagerServer);
   
    return true;
}

void deregister_farmMask(cMessage* msg_req,DSRIManagerServer* dsriManagerServer)
{
    //for(map<string, farmStructure*>::iterator c_iter= farmMask.begin(); c_iter !=farmMask.end(); ++c_iter)
  //  int pIdPosition= (c_iter->first).find("/");
   // std::string processId = (c_iter->first).substr(0,pIdPosition);
    //map<string,farmStructure*>::iterator it = farmMask.find(processId);

    //while (it!=farmMask.end())
   // {
     //   farmMask.erase(it);
       // it = farmMask.find(processId);
   
    //}
    dsriManagerServer->deregister_maskFarm (msg_req->senderId);
}

bool deregister_application(cMessage* msg_req,DSRIManagerServer* dsriManagerServer)
{
    map<string,app_info*>::iterator it = registered_apps.find(msg_req->senderId);
    if(it!=registered_apps.end()) 
    {
        dsriManagerServer->flush_sensor_file(msg_req->senderId);
        registered_apps.erase(it);
    }

    set<string>::iterator iter = currently_selected_apps.find(msg_req->senderId);
    if(iter!=currently_selected_apps.end())
    {
        currently_selected_apps.erase(iter);
    }

    set<string>::iterator sus_iter = suspended_apps.find(msg_req->senderId);
    if(sus_iter!=suspended_apps.end())
    {
        suspended_apps.erase(sus_iter);
    }
    deregister_farmMask(msg_req,dsriManagerServer);
    dsriManagerServer->releaseResources(msg_req->senderId);

}

bool process_application(cMessage* msg_req, DSRIManagerServer* dsriManagerServer)
{
    if(registered_apps.at(msg_req->senderId)->activation_status)
    {
        (registered_apps[msg_req->senderId])->sensor= msg_req->content;
        //std::cout<< "reciveMessage================== **********:"<<(msg_req->content) << std::endl;
        set<string>::iterator iter = currently_selected_apps.find(msg_req->senderId);
       
        dsriManagerServer->update_application_information(msg_req->senderId);

        if(iter==currently_selected_apps.end())
            currently_selected_apps.insert(msg_req->senderId);
        // try to upgrade dlt from previous movement 
        if((registered_apps[msg_req->senderId])->DLTUpdateNeeded)
        {
            cMessage* msg_update=new cMessage;
            msg_update->senderId=msg_req->senderId;
            msg_update->content=dsriManagerServer->improve_btn_path(msg_req->senderId);
	      //  std::cout<< "DLT UPDATE **********:"<<(msg_update->content) << std::endl;
            msg_update->sendMessage();
            (registered_apps[msg_req->senderId])->DLTUpdateNeeded=false;
        }
        // try to move to gpu
        if(dsriManagerServer->improve_btn_node(msg_req->senderId))
        {
            cMessage* msg_update=new cMessage;
            msg_update->senderId=msg_req->senderId; 
            msg_update->content=(registered_apps[msg_req->senderId])->actuator;
            msg_update->sendMessage();
        }
        if(registered_apps[msg_req->senderId]->has_useless)
                resume_suspended_apps(dsriManagerServer);

        return true;

    }else return false;
}
int main (int argc, char * argv[]) {
    prepareZMQconnection();
    createApplicationDatabase();
    DSRIManagerServer * dsriManagerServer = new DSRIManagerServer();
    std::string const_address;
    if(argc>1)
    {
       std::cout << "****************I am here"<< std::endl;
        const_address=argv[1];
        dsriManagerServer->environmentConstraint(const_address);
    }
    else 
    {
        
        char result[ PATH_MAX ];
        size_t count = readlink( "/proc/self/exe", result, PATH_MAX );
        std::string exe_path= std::string( result, (count > 0) ? count : 0 );
        unsigned found = exe_path.find_last_of("/");
        std::string path = exe_path.substr(0, found);
        const_address=path + "/envConst/env_const.json";
        std::cout << "following environmentConstraint file has been applied : "<<const_address<< std::endl;
        dsriManagerServer->environmentConstraint(const_address);
    }
    zmq::pollitem_t items_server [] = {{socket_server,  0, ZMQ_POLLIN, 0 }};
    double start_time=get_current_time();
    while (true)
    {
        zmq::poll (&items_server [0], 1, POLL_WAIT); //this one
        if (items_server [0].revents & ZMQ_POLLIN) 
        {//this one
            //std::cout<< "inside the if"<<std::endl;
            cMessage* msg_req= new cMessage;
            msg_req->reciveMessage();
            
           //register for first time
            if((registered_apps.find(msg_req->senderId)==registered_apps.end()))
            {
               register_application(msg_req,dsriManagerServer);
            }
            else if(strcasecmp(msg_req->content.data(), "END")==0)
            {  
               
                // here the sensor write must happened to the system
                std::cout << "Application: " << msg_req->senderId <<"+++++++++++++++++++++++++++_____________has finished______________++++++++++++++++++++++++++++++++++++"<< std::endl;
               deregister_application(msg_req,dsriManagerServer);
               // wake up any sleep application based on priority
               resume_suspended_apps(dsriManagerServer);
               
            }
            else 
            {
               
               process_application(msg_req,dsriManagerServer);

            }
            map<string,app_info*>::iterator it = registered_apps.find(msg_req->senderId);
            if((it!=registered_apps.end()) && 
                ((get_current_time()-((it->second)->update_time))>dlt_period))
            {   
    
              //  if(pr==VARIABLE)
               // {   
                //    calculate_age(msg_req->senderId); // for currently selected apps which are not suspended
                 //   resume_suspended_apps(dsriManagerServer);
                //}

                (it->second)->update_time=get_current_time();
                (it->second)->DLTUpdateNeeded=true; /*take it uncomment to have dynamic dlt*///should check if static or dynamic required preferably for each node
            }

            //flush_sensor(msg_req->senderId);
        
            if(((get_current_time()-start_time)>pr_period)&& (pr==VARIABLE))
            {   
                   
                calculate_age(); // for currently selected apps which are not suspended
                resume_suspended_apps(dsriManagerServer);
                start_time=get_current_time();

            }
    
        } 
        // here the aging process will call to aged the applications
          std::cout<< "wating for clients"<<std::endl; 
    }
    return 0;
}
