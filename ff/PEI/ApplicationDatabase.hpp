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
 
#ifndef _APPLICATIONDATABASE_HPP
#define _APPLICATIONDATABASE_HPP
#include <vector>
#include <string>
#include <map>
#include <set>
#include <sys/stat.h>
//#include <unistd.h>
//#include <sys/types.h>
//#include <algorithm>
#include <fstream>
#include <linux/limits.h>
#include <iterator>
using namespace std;


enum SWITCH_MODE {AD_HOC=0, AVERAGE};
SWITCH_MODE sMode = AD_HOC;
enum APP_PRIORITY {FIX=0, VARIABLE};
APP_PRIORITY pr = VARIABLE;
double pr_period= 5;
double dlt_period= 5;

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


struct app_info
{
  app_info()
  {
  	DLTUpdateNeeded=true;
    activation_status=true;
    has_useless=false;
    update_time=0;
  }
  int priority;
  string sensor;
  string actuator;
  string btn_node;
  string btn_path;
  bool DLTUpdateNeeded;
  bool activation_status;
  string application_file_id;
  bool has_useless;
  std::set<std::string> useles_node_address;
  double update_time;


};
struct farmStructure
{
  farmStructure(int tnw,string pa):totalNumWorkers(tnw), parent_address(pa), numSuspendedCount(0){}
  set<string> cmpAddress;
  int numSuspendedCount;
  int totalNumWorkers;
  string parent_address;
};
string databaseAddress;
static map<string, app_info*> registered_apps;// all of the application running
static set<string> currently_selected_apps; // currently apps going to be considered to edit
static set<string> modified_actuators_apps; // app has been swapped or switched their resources
static set<string> suspended_apps; // app has been suspended due to lack of resources 
static vector<string> dlt_distributed;// for plotting dlt diagram at the end. 
static map<string, farmStructure*> farmMask;
static int masked_cpu=0;
static int masked_gpu=0;
struct compare_value
{
    compare_value(){}
    bool operator()(std::string first_app, std::string second_app)
    {
      return ((registered_apps.at(first_app)->priority)<=(registered_apps.at(second_app)->priority));
    }
};


struct compare_value_farm
{
    compare_value_farm(){}
    bool operator()(std::string first_app, std::string second_app)
    {
      return ((registered_apps.at(first_app.substr(0,first_app.find_first_of("/")))->priority)<=(registered_apps.at(second_app.substr(0,second_app.find_first_of("/")))->priority));
    }
};

std::vector<std::string> prioritySort(std::set<std::string>& set_apps)// policy for priority app it can change based on throughput or etc
{   
    compare_value cmp;
    std::vector<std::string> current_vector(set_apps.begin(), set_apps.end());
    // std::cout<<"CURENT VECTOR SIZE:_+_+_+_+_+_"<<current_vector.size()<<std::endl;
    //std::cout<<"REG_APP_ SIZE:_+_+_+_+_+_"<<registered_apps.size()<<std::endl;
    std::sort(current_vector.begin(),current_vector.end(), cmp);
    return current_vector;
};

std::vector<std::string> prioritySort_farm(std::set<std::string>& set_apps)// policy for priority app it can change based on throughput or etc
{   
    compare_value_farm cmp;
    std::vector<std::string> current_vector(set_apps.begin(), set_apps.end());
    // std::cout<<"CURENT VECTOR SIZE:_+_+_+_+_+_"<<current_vector.size()<<std::endl;
    //std::cout<<"REG_APP_ SIZE:_+_+_+_+_+_"<<registered_apps.size()<<std::endl;
    std::sort(current_vector.begin(),current_vector.end(), cmp);
    return current_vector;
};

void createApplicationDatabase()
{
    char result[ PATH_MAX ];
    size_t count = readlink( "/proc/self/exe", result, PATH_MAX );
    std::string exe_path= std::string( result, (count > 0) ? count : 0 );
    unsigned found = exe_path.find_last_of("/");
    std::string path = exe_path.substr(0, found);
    std::string ProfilingFolder= std::string("Profiling/");  
    struct stat st = {0};
    
    if (stat(ProfilingFolder.c_str(), &st) == -1) 
    {
      mkdir(ProfilingFolder.c_str(), 0700);
    }

    databaseAddress=path + "/" +ProfilingFolder;
    std::cout << "following database has been created : "<<databaseAddress<< std::endl;
   
    //unsigned found = ff_manager::path.find_last_of("/");
  //  ff_manager::jnDirectories = ff_manager::ProfilingFolder +ff_manager::applicationName /*ff_manager::path.substr(found+1)*/ + "/";
   // if (stat(ff_manager::jnDirectories.c_str(), &st) == -1) 
   // {
    //  mkdir(ff_manager::jnDirectories.c_str(), 0700);
    //} 

}

void flush_sensor(std::string appId)
{

  if(!((registered_apps.at(appId)->sensor).empty()))
  {
    std::string filename= databaseAddress + registered_apps.at(appId)->application_file_id; 
    
    std::ofstream out(filename.c_str());
    out <<(registered_apps.at(appId)->sensor) ;
    out.close();

  }
  if(!((registered_apps.at(appId)->actuator).empty()))
  {
  	 std::string actuator_name ("actuator");
  	 std::string filename= databaseAddress + registered_apps.at(appId)->application_file_id + actuator_name; 
    
    std::ofstream out(filename.c_str());
    out <<(registered_apps.at(appId)->actuator) ;
    out.close();
  }
  // from here for dlt diagram
//if(strcasecmp((registered_apps.at(appId)->application_file_id).c_str(), "_users_mehdi_Thesis_fastflow-2.0.2.10-VIP_examples_Bilateral-Denoise_build_nested.json")==0){
if(strcasecmp((registered_apps.at(appId)->application_file_id).c_str(), "_users_mehdi_Thesis_fastflow-2.0.2.11-VIP_examples_simple-convolution_Lib_version_build_nested.json")==0){
  
  std::string filename_dia= databaseAddress +  "file_DLT.txt"; 
  std::ofstream out_dia(filename_dia.c_str());
  //std::ostream_iterator<std::string> output_iterator(out,"\n");
  // std::copy(dlt_distributed.begin(), dlt_distributed.end(),output_iterator);
      for(int i=0; i< dlt_distributed.size();++i)
	{
	  out_dia << dlt_distributed.at(i);//<< "\n";
	  //out<< "\n"
	}
      out_dia.close();
}
  // till here must be commented for real app
};


 std::string read_json_file(std::string filename)
 {
    std::string contents;
    std::ifstream in(filename.c_str(), std::ios::in | std::ios::binary);
    if (in)
    {
      //std::string contents;
      in.seekg(0, std::ios::end);
      contents.resize(in.tellg());
      in.seekg(0, std::ios::beg);
      in.read(&contents[0], contents.size());
      in.close();
      if(!(contents.empty()))
      {
        try
        {
          //registered_apps.at(appId)->sensor=contents;
          //n = libjson::parse(contents);
          return contents;
        }
        catch(std::invalid_argument e)
        {
          std::cout<< "data corrupted: using default value" <<std::endl;
          //exit(EXIT_FAILURE);
          contents.clear();
        }
      }
    }

    return contents;
  }
  
  void load_sensor(std::string appId)
  {
  
    std::string filename= databaseAddress + registered_apps.at(appId)->application_file_id; 
    std::cout<< "this is file name"<<filename<< std::endl;
    std::string c =read_json_file(filename);
    if(!(c.empty()))
      registered_apps.at(appId)->sensor=c;

/*  std::ifstream in(filename.c_str(), std::ios::in | std::ios::binary);
  if (in){
    std::string contents;
    in.seekg(0, std::ios::end);
    contents.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], contents.size());
    in.close();
    if(!(contents.empty())){
      try{
        registered_apps.at(appId)->sensor=contents;
        //n = libjson::parse(contents);
      }catch(std::invalid_argument e){
        std::cout<< "data corrupted: using default value" <<std::endl;
        //exit(EXIT_FAILURE);
      }
    }
  }*/
  }


#endif
