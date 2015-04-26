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
 
#include <ff/PEI/DSRIManagerNode.hpp>
using namespace ff;
pthread_t ff_manager::manger_th_handle=0;
int ff_manager::rc_called=-1;

  JSONNode ff_manager::read_json_file(const char * filename)
  {
  
  std::ifstream in(filename, std::ios::in | std::ios::binary);
  JSONNode n(JSON_NULL);
  if (in){
    std::string contents;
    in.seekg(0, std::ios::end);
    contents.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], contents.size());
    in.close();
    if(!(contents.empty())){
      try{
        n = libjson::parse(contents);
      }catch(std::invalid_argument e){
        std::cout<< "bad JSON file is sent."<<std::endl;
        exit(EXIT_FAILURE);
      }
    }
  }
  return n;
}
std::string ff_manager::getApplicationName()
{
 return ff_manager::applicationName;
}

std::string ff_manager::getexepath()
  {//yes
    return ff_manager::path;
  };

ff_manager::ff_manager()
{
   
    ff_manager::root=NULL;
    ff_manager::stop=false;
    ff_manager::processId =getpid();
    ff_manager::priority=0;
    ff_manager::sample_mode=0;
    
    std::cout<< "this is my id :" << ff_manager::processId<<std::endl;
   
  };

ff_manager::~ff_manager()
{
   std::cout<<"++++++++++++++++++++++++++++++called"<<std::endl;
  #if defined(FASTFLOW_OPENCL)
   clEnvironment::instance()->cleanall();
  #endif
    ff_manager::finalise();
    std::cout<<"++++++++++++Finnished"<< std::endl;
  //printf("here\n");
}
void ff_manager::finalise()
{
  if (rc_called!=0)
  {
    ff_manager::stop=true;
    #if defined(TRACE_FASTFLOW)
    rc_called =pthread_join(ff_manager::manger_th_handle, NULL);
    #endif

  //  std::cout<<"called"<<std::endl;
  }
};

void ff_manager::set_sample_mode(int s_m){
  ff_manager::sample_mode=s_m;
};
void ff_manager::set_dashboard(ff_node* ff_root, bool doM=true, bool doSch=true, 
  const char* actuator_name ="actuator_", const char* sensor_name="sensor_", unsigned long long val=0 )
{
    ff_manager::doMap=doM;
    ff_manager::doSchedule=doSch;
    ff_manager::root=ff_root;
    ff_manager::dataSize= val;

#if defined(TRACE_FASTFLOW)
    ff_manager::modular_mode= sample_mode;
    ff_manager::root->set_modular_mode(ff_manager::modular_mode);
    ff_manager::root->setDataSize(dataSize);
#endif
    char result[ PATH_MAX ];
    size_t count = readlink( "/proc/self/exe", result, PATH_MAX );
    ff_manager::application_file_Id= std::string( result, (count > 0) ? count : 0 );
  };

  void ff_manager::set_priority(int pri){
    if(pri<0 || pri>99) {
      std::cout<< "Priority must be between 0 and 99. the defaut priority has been set here."<<std::endl;
      ff_manager::priority=0;
    } else ff_manager::priority=pri;

  };


  void ff_manager::profile_and_tune()
  {

    #if defined(ZMQ_DSRI)
    prepareZMQconnection(ff_manager::processId);
    #endif

    if(ff_manager::root)
    {
     // ff_manager::apply_actuators();
      #if defined(ZMQ_DSRI)
      ff_manager::registerApplication();
      #endif
      if(!(manger_th_handle))
      {
        if (pthread_attr_init(&(attr))) 
        {
          perror("pthread_attr_init: pthread can not be created.");
          exit (EXIT_FAILURE);
        }
        if (pthread_create(&(manger_th_handle), &(attr), manager_thread, this) != 0) 
        {
          perror("pthread_create: pthread creation failed.");
          exit (EXIT_FAILURE);
        }    
        ff_manager::root->registerEndCallback(thread_finaliser, this);
      }
      else
      {
        std::cout<< "ERROR:: You cannot profile more than one skeleton tree/subtree"<<std::endl;
        exit (EXIT_FAILURE);
      }
    }
    else
    { 
      std::cout<< "WARNING: TRACE_FASTFLOW is set true, but no node has been selected to profile. No actuator and sensor will be generated." <<std::endl;
    }
  };
  //threshold_pressure will say that how much of the free ram shoub be used for the program. the default is half. 
double ff_manager::get_memory_throtling_threshold(int threshold_pressure=2)
{
  
  struct sysinfo info;
  if (sysinfo(&info) != 0)
     printf("sysinfo: error reading system statistics");
  return ((((double)(1.0*info.freeram))/info.totalram)/threshold_pressure);

}
uint ff_manager::getProcessId()
{
  return ff_manager::processId;
}
#if defined(ZMQ_DSRI)
void ff_manager::registerApplication()
{
  
    std::cout<< "Warning: No sensors file found using default allocation..."<<std::endl;
    JSONNode default_sensors=ff_manager::root->get_sensors();
    default_sensors.push_back(JSONNode("Sensor_Output_Filename_Name",ff_manager::application_file_Id));
    default_sensors.push_back(JSONNode("Priority",ff_manager::priority));
    sendRequestFile(default_sensors.write_formatted());

    std::string msg_rep =receiveReplyFile();// because the static one is one single connection it should not be polling
    JSONNode default_actuator = libjson::parse(msg_rep);
    ff_manager::root->set_actuators(default_actuator);
    std::cout<<default_actuator.write_formatted()<<std::endl;
}
#endif


// the 2 functions
static void thread_finaliser(void * arg)
{
 ((ff_manager*)arg)->finalise();
  //printf("is called");
}
#if defined(ZMQ_DSRI)
void prepareZMQconnection(uint processId)
{
  //  Prepare our context and socket
  std::cout << "Connecting to DSRI Manager server…" << std::endl;
  std::string my_id = static_cast<ostringstream*>( &(ostringstream() << processId) )->str();
  
    //char result[ PATH_MAX ];
    //size_t count = readlink( "/proc/self/exe", result, PATH_MAX );
    //std::string exe_path= std::string( result, (count > 0) ? count : 0 );
    //std::cout << "mypath: "<< exe_path<< std::endl;
    //unsigned found = exe_path.find_last_of("/");

  //temporary
    //    std::string path = exe_path.substr(0, found);
     //   std::string path = ("/users/mehdi/Thesis/fastflow-2.0.2-paraphrase/ff/PEI");
    //exe_path.substr(0, found);
     //temporary  
    std::string path = std::string("/tmp");  
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
    }
   // else 
   // {
      
   // }

   // struct stat st = {0};
    
   // if (stat(ZMQFolder.c_str(), &st) == -1) 
   // {
    //  mkdir(ZMQFolder.c_str(), 0700);
   // }

    std::string zqp="ipc://" + path + "/" +ZMQFolder + "ZMQWorkspace.ipc";
    socket_client.setsockopt( ZMQ_IDENTITY, my_id.data(), my_id.size()); // A must be changed by process id
    socket_client.connect(zqp.c_str());
}

int sendRequestFile(std::string content)
{   
   int rc =zmq_send (socket_client, content.data(), content.size() , ZMQ_DONTWAIT);
   return rc;
}

std::string receiveReplyFile()
{
  std::string content = s_recv (socket_client);  // Here we only recive the actuator this is the actuator file send by servever  
  return content;
}
#endif


double get_current_time(){
  static int start = 0, startu = 0;
  struct timeval tval;
  double result;

  if (gettimeofday(&tval, NULL) == -1)
    result = -1.0;
  else if(!start) {
    start = tval.tv_sec;
    startu = tval.tv_usec;
    result = 0.0;
  }
  else
    result = (double) (tval.tv_sec - start) + 1.0e-6*(tval.tv_usec - startu);

  return result;
}


static void * manager_thread(void * arg) 
{
  ff_manager * manager = (ff_manager *) arg;
  double start_time=get_current_time();
  set_memory_throtling_threshold(manager->get_memory_throtling_threshold()); 

#if defined(TRACE_FASTFLOW)
 
  int i=0;
  JSONNode sensors;
  JSONNode actuators;
 
  while(true)
  {
    if((get_current_time()-start_time)>=PERIOD)
    {
      manager->root->set_modular_mode(manager->modular_mode);
      sensors=manager->root->get_sensors();
      sensors.push_back(JSONNode("Sensor_Output_Filename_Name",manager->application_file_Id));

      #if defined(ZMQ_DSRI)  
      int status =sendRequestFile(sensors.write_formatted());
      #endif
      start_time=get_current_time();
    }
    
  #if defined(ZMQ_DSRI)
    
    zmq::poll (&items [0], 1, POLL_WAIT); 
    
    if (items [0].revents & ZMQ_POLLIN) 
    {
      std::string msg_rep =receiveReplyFile();

      if (msg_rep.find("END") != std::string::npos)
      {
        std::cout<<"__________________________________________this is the message_________________________________________________: "<<msg_rep <<std::endl;
      }
     
      else if (msg_rep.find("Resume") != std::string::npos) 
      {
        std::cout<<"Resumed*************************************" <<std::endl;
        actuators = libjson::parse(msg_rep);
        manager->root->set_actuators(actuators);
        resume_broadcast_node();
      }
      else if (msg_rep.find("Suspend") != std::string::npos)
      {
        std::cout<<"suspended*************************************" <<std::endl;
        actuators = libjson::parse(msg_rep);
        manager->root->set_actuators(actuators);
      }
      else
      { 
        std::cout<<"Acturator recieved*************************************" <<std::endl;
        actuators = libjson::parse(msg_rep);
        std::cout<<msg_rep<<std::endl;
        manager->root->set_actuators(actuators);
      }
    }
  #endif
   
    if(manager->stop)
    {  
      #if defined(ZMQ_DSRI)  
      sendRequestFile("END");
      std::cout<< "I sent"<<std::endl;
      #endif

      break;
    }
  }
  
#else

  while (true)
  {
    if(manager->stop)
    {
      std::cout<<"finished"<<std::endl;
      break;
    }
  }
#endif
  pthread_exit(NULL);
  return NULL;
};
