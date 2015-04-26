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
 
#ifndef _DLT_
#define _DLT_
#include <ff/cycle.h>
#include <vector>
#include <cmath>
 #include "ff/PEI/libjson/libjson.h"
#include "ff/PEI/libjson/dependency.hpp"
 #include <sstream>

//namespace ff {
enum STATUS{OFF=0, ON};

struct WorkerInfo{
	WorkerInfo(int wId, double serviceTime, double zTime, STATUS s):
	wId(wId), serviceTime(serviceTime), zTime(zTime), s(s), timeBasedLoad(0.0){
		//serviceTime = ((1.0*eSvcTime)/taskcnt);
	}
	int wId;
	ticks eSvcTicks;
	ticks zTicks;
	STATUS s;
	double serviceTime;
	double zTime;
	double timeBasedLoad; // share of the stream load which should go to the processor based on service time
};

class DLT{
private:
	bool calculateLoad();
   	std::vector<WorkerInfo*> workerList;

public:
  	DLT();
  	//void get_workload(std::vector<WorkerInfo*>);
  	JSONNode get_workload(JSONNode&);
  	WorkerInfo * getWorkerInfo(const JSONNode& jn);
};
#include <ff/PEI/DLT.cpp>
//}

#endif

