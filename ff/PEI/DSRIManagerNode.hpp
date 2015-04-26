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

 // This is a DSRI manager Client. It is an Interface which comunicate with its Own DSRI server located in each node.
#ifndef _FF_MANAGER_HPP_
#define _FF_MANAGER_HPP_

#include <ff/farm.hpp>
#include <ff/pipeline.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <ff/PEI/mem_man.h>
#include <map>
#include <zmq.hpp>
#include <zhelpers.hpp>
#include <fstream>
#include <ff/cycle.h>
#include "ff/PEI/libjson/libjson.h"
#include "ff/PEI/libjson/dependency.hpp"
#include <ff/PEI/cl_environment.hpp>


//#define ZMQ_DSRI 0; 
namespace ff {
#if defined(ZMQ_DSRI)  
static void           prepareZMQconnection(uint);
static int           sendRequestFile (std::string);
static std::string    receiveReplyFile();
#endif

static void*          manager_thread(void * arg);
static void           thread_finaliser(void * arg);
#define PERIOD 1
#define POLL_WAIT 100


#if defined(ZMQ_DSRI)
zmq::context_t context (1);
zmq::socket_t socket_client (context, ZMQ_DEALER);
//for poll it is here because i want to be generated only once
zmq::pollitem_t items [] = {
  {socket_client,  0, ZMQ_POLLIN, 0 }
};
#endif


class ff_manager{
    friend void*          manager_thread(void *arg);
    friend void           thread_finaliser(void * arg);
    friend double         get_current_time();
#if defined(ZMQ_DSRI)
    friend void           prepareZMQconnection(uint);
    friend int           sendRequestFile (std::string);
    friend std::string    receiveReplyFile();
#endif
 public:
  ff_manager();
  ~ff_manager();
  void                set_dashboard(ff_node*, bool, bool, const char* , const char *, unsigned long long);
  void                profile_and_tune();
  void                finalise();
  std::string         getexepath();
  std::string         getApplicationName();
  void                set_priority(int priority);
  uint                getProcessId();
  void                set_sample_mode(int);
  #if defined(ZMQ_DSRI)
  void                registerApplication();
  #endif
 private:
  bool                stop;
  static int          rc_called;
  static pthread_t		manger_th_handle;
  pthread_attr_t      attr;
  unsigned long long	dataSize;
  std::string         actuators_file_name;
  std::string         sensors_file_name;
  ff_node * 		      root;
  std::string         jnDirectories;
  bool                doMap;
  bool          			doSchedule;
  unsigned long       modular_mode;
  std::string         ProfilingFolder;
  std::string         applicationName;
  std::string         path;
  uint                processId;
  JSONNode            read_json_file(const char *);
  double              get_memory_throtling_threshold(int);
  std::string         application_file_Id;
  int                 priority;
  int                 sample_mode;

};
#include <ff/PEI/DSRIManagerNode.cpp>

#define GET_4TH_ARG(arg1, arg2, arg3, arg4, ...) arg4
#define PROFILE_MACRO_CHOOSER(...) \
    GET_4TH_ARG(__VA_ARGS__, PROFILE_2, \
                PROFILE_1, PROFILE_0, )

#define PROFILE(...) PROFILE_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

#define PROFILE_0(name) ff_manager  m_ffManager; m_ffManager.set_dashboard(name); m_ffManager.set_sample_mode(0); m_ffManager.set_priority(0); m_ffManager.profile_and_tune();
#define PROFILE_1(name, sm) ff_manager  m_ffManager; m_ffManager.set_dashboard(name); m_ffManager.set_sample_mode(sm); m_ffManager.set_priority(sm); m_ffManager.profile_and_tune();
#define PROFILE_2(name, sm,pr) ff_manager  m_ffManager; m_ffManager.set_dashboard(name); m_ffManager.set_sample_mode(sm); m_ffManager.set_priority(pr); m_ffManager.profile_and_tune();
//#define PROFILE_1(name, doMap) ff_manager  m_ffmanagerfManager; m_ffManager.set_dashboard(name, doMap); m_ffManager.profile_and_tune();
//#define PROFILE_2(name, doMap,doSch) ff_manager  m_ffManager; m_ffManager.set_dashboard(name, doMap,doSch); m_ffManager.profile_and_tune();


// CONSTRUCT_SKELETON(address, __VA_ARGS__) address is the jn file address and args are the componentns ACTION(name, __VA_ARGS__) 
    //name is the name of the action and args are the arguments if needed. 
    // in CONSTURCT_SKELTON the m_ffmanager is going to be created and the build from it is going to be caled also an std::map
    //should be created to keep the name of all function in the system. each svc need to have the instance function to instantiate themselves.
 // there is no need for any extra class in that case only a reverse function which generate the code from json.
}
#endif /* _FF_MANAGER_HPP_ */
