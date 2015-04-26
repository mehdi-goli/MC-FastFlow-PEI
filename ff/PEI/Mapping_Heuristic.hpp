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

#ifndef _MAPPINGHEURISTIC_HPP
#define _MAPPINGHEURISTIC_HPP

#include <ff/PEI/Environment.hpp>
#include <ff/PEI/ApplicationDatabase.hpp>
#include "ff/PEI/libjson/libjson.h"
#include "ff/PEI/libjson/dependency.hpp"
#include <sstream>



class  mappingHeuristics
{
public:
	mappingHeuristics();
	template<typename T>
	bool editActuator(JSONNode&, std::string, std::string, T);
	int get_max_ocl_task_per_device(int);
	int findBestGPUDeviceByType(std::string , std::string );
	int findBestGPUDevice(std::string /*, std::string*/);
	bool moveToGPU(std::string);
	// allocate cpu id must change it must update must of the time and if it is not exist add to it
	int findBestCPUCore(std::string, double, int);// this is the Multilevel queues level priority heuristic
	int get_gpu_device(std::string);
	int suspend_application(std::string);
	int findWorkerNumbers(JSONNode&, std::string);
	void unMaskComponentDevice(std::string, int=1);
	int maskComponentDevice(std::string,int=1);
	int try_masking(std::string);
	bool try_unmasking(std::string);
	std::string min_val(int , int );
	void deregister_masked(std::string);
	bool SwappingYourself(std::string, std::string);
	void getParrentPath(std::string , std::string & , std::string& );
	void get_object(JSONNode& , std::string , JSONNode&);




};

#include <ff/PEI/Mapping_Heuristic.cpp>

#endif