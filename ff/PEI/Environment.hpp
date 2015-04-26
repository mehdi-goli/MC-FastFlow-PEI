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
 
#ifndef _ENVIRONMENT_HPP
#define _ENVIRONMENT_HPP
#include <ff/PEI/architecturalInfo.hpp>
#include <ff/PEI/ApplicationDatabase.hpp>
#include <set>
#include <cstdlib>
#include <ctime>


//Component name and priority must be added 
// enum APP_STATUS{NON=0, CHANGE, SLEEP};

 /*struct app_info
 {
  app_info(){}
  APP_STATUS status;
  int priority;
 };*/
struct cpuMapInfo
{
  cpuMapInfo()
  {
    total_time =0;
    total_tick=0;
    idle_tick=0;
    load_percentage=0;

  }
  // the first is the app_name por process_id
  // the second is the number of app_percore
 // std::map<string, int> app_per_core;
  // the first is component name
  //the second is total service time for that application
  std::map <string, int> allocatedapplications;
  std::map<string, double> componentsServiceTime;
  //total time for all application allocated to this node
  double total_time; 
  unsigned long long total_tick;
  unsigned long long idle_tick;
  double load_percentage;
};

struct gpuMapInfo
{ // the first is application name
  //the second is total number of running components form this applicated dedicated to this gpu
  gpuMapInfo()
  {
    total_app=0;
  }
  std::map <string, int> allocatedapplications;
  std::set<string> allocatedComponents;
  std::set<string> inActiveallocatedComponents;

  // total number of running components of all application dedicated to this gpu.
  int total_app; 
};

class Environment
{
private: 
  static Environment* m_environment;
  //std::map<std::string, app_info*> application_registery;
 // int UpdateCPUMappingArray(std::string , double );
  int CPU_LIMIT;
  int GPU_LIMIT;
   
  Environment();
public:
  cpuMapInfo** cpuMappingArray; // an array containing all of the cpu cores 
  gpuMapInfo** gpuMappingArray;// an array containing all of the ocl enable devices
  nodeInfo * node_info; // Node architecture
  static Environment* get_instance();
  // bool update all cpu and gpu nodes based on the last service time
  bool  updateCPUComponetServiceTime(std::string , double  ); 
  bool  allocateGPUId(int, std::string);
  bool  allocateCPUId(int, std::string , double , int );
  int   getComponentCPUCore(std::string);
  int   getComponentGPUDevice(std::string );
  int   get_rand(int);
  void  releaseResources(std::string);
  void  deallocateCPUId(std::string);
  void  deallocateGPUId(std::string );
  void  deallocateGPUComponent(std::string );
  void  inactivedeallocateGPUComponent(std::string );
  void  deallocateCPUComponent(std::string  );
  //bool  swapCPUComponent(std::string, std::string, std::string, std::string );
 // bool  swapGPUComponent(std::string, std::string, std::string, std::string );
  //int   updateActualCPULoad();
  std::string getComponent(std::string, std::string);
  std::string getInactiveComponent(std::string, std::string);
  std::string getComponent_rand(std::string , std::string);
  void  activateGPUComponent(int , std::string);
  void  inActiveGPUId(int ,std::string);
  bool  inActiveallocateGPUId(int, std::string);
  int   get_cpu_limit();
  void  set_cpu_limit(int);
  int   get_gpu_limit();
  void  set_gpu_limit(int);

};
#include <ff/PEI/Environment.cpp>
#endif