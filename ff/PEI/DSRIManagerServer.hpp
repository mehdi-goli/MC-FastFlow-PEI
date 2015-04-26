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
#ifndef _DSRIMANAGERSERVER_
#define _DSRIMANAGERSERVER_

#include <ff/PEI/DLT.hpp>
#include <ff/PEI/Mapping_Heuristic.hpp>
#include <ff/PEI/ApplicationDatabase.hpp>
#include "ff/PEI/libjson/libjson.h"
#include "ff/PEI/libjson/dependency.hpp"
#include <ff/cycle.h>
#include <vector>
#include <cmath>
#include <algorithm>

#define USELESS_NODE 100000
#define NOT_OCLNODE 10000
using namespace std;

class DSRIManagerServer{

private:

  //bool DLT_STATUS;
  //STATUS get_masking(const JSONNode & , const JSONNode &);
  ticks getFarmEffectiveTick(JSONNode& , unsigned long , unsigned long );
  double getFarmEffectiveTime(JSONNode&, unsigned long, unsigned long);
  ticks getEffectiveTick(const JSONNode& );
  double getEffectiveTime(const JSONNode& );
  ticks getEffectiveLbTick(const JSONNode& );
  double getEffectiveLbTime(const JSONNode& );
  
  double get_arrival_rate(double , unsigned long , unsigned long , double);
  JSONNode get_maskArray(std::vector<STATUS>);
  JSONNode parse_sensors(const JSONNode& sensors, std::string app_name);
  JSONNode find_buttleneck_node(const JSONNode&);
  JSONNode find_buttleneck_path(std::string, const JSONNode&, double);
  JSONNode parse_DLT_sensor(const JSONNode& , std::string);
  template<typename T> std::string toString(T); 
  double get_last_serviceTime(const JSONNode& );
  //void editWorkerStatus(JSONNode&, STATUS);

public:
  DSRIManagerServer();
  void releaseResources(std::string);
  std::string build_actuators(std::string);
  JSONNode build_Architectural_info(nodeInfo* );
  bool improve_btn_node(std::string);
  std::string improve_btn_path(std::string);
  void update_application_information(std::string);
  std::string get_action(std::string);
  void load_sensor_file(std::string,std::string);
  void flush_sensor_file(std::string);
  //std::string get_masking(std::string);
  std::vector<STATUS>  get_masking(std::string , int , std::string );
  bool try_unmaskApp(std::string , bool&);
  void deregister_maskFarm(std::string);
  void environmentConstraint(std::string);
  bool remove_useless_unmask(std::string);
  bool is_mask(std::string);
  void construct_constraint(JSONNode&);
};
#include <ff/PEI/DSRIManagerServer.cpp>
#endif
