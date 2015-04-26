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
 
#include <ff/PEI/DLT.hpp>
//using namespace ff;
  DLT::DLT(){}

  WorkerInfo * DLT::getWorkerInfo(const JSONNode& jn)
  {//yes
      int wid=-1;
      //ticks eTicks=0;
      STATUS s=OFF;
      double serviceTime=0;
      JSONNode::const_iterator it =jn.find_nocase("Node_Address");
      if(it!=jn.end()){
        std::string cmpNode_address=it->as_string();
        unsigned found = cmpNode_address.find_last_of("/");
        std::string url_path=cmpNode_address.substr(0,found);
        std::string cmpId= cmpNode_address.substr(found+1);
        std::istringstream ( cmpId ) >> wid;
      }
     // it =jn.find_nocase("Effective_Ticks");
     // if(it->as_string()!=""){
      //  eTicks = it->as_int();
      //}

      it =jn.find_nocase("Service_Time");
      if(it!=jn.end() && it->as_string()!=""){
        serviceTime = it->as_float();
      }

      it =jn.find_nocase("Status");
      if(it!=jn.end()  && it->as_string()!="" && it->as_int()==1){
        s = ON;
      }
      return new WorkerInfo(wid,serviceTime,0,s);
  };


 
  JSONNode DLT::get_workload(JSONNode& workers /*std::vector<WorkerInfo*> wInfos*/){
    for(unsigned int i = 0; i < workers.size(); ++i)
    {
      
      DLT::workerList.push_back(getWorkerInfo(workers[i].as_node()));
      //workers[i].erase(workers[i].find_nocase("Effective_Ticks"));
    }
   // DLT::workerList =wInfos;
    //int i=0;
    //for (std::vector<WorkerInfo*>::iterator it = DLT::workerList.begin() ; it != DLT::workerList.end(); ++it){
      //      std::cout << "Number : " <<i <<" : "<<(*it)->eSvcTicks<< std::endl;
        //    i++;
      //    }
    DLT::calculateLoad();
    JSONNode warr(JSON_ARRAY);
    warr.set_name("Workload");
    for(std::vector<WorkerInfo*>::iterator it= workerList.begin(); it!=workerList.end(); ++it)
    {
      warr.push_back(JSONNode("",(*it)->timeBasedLoad));
    }
    return warr;
  }  
  bool DLT::calculateLoad(){
  //  ticks baseload=0.0;
    double timeBaseLoad=0.0;
   // ticks currentTick=0;
    double currentTime=0;
    for (std::vector<WorkerInfo*>::iterator it = DLT::workerList.begin() ; it != DLT::workerList.end(); ++it){
     // if(((*it)->s)==ON){
      if((((*it)->s)==ON) && (((*it)->serviceTime)>0)) //{
       // if(((*it)->serviceTime)>0)
        {
         // currentTick =(*it)->eSvcTicks;
          currentTime=(*it)->serviceTime; 
          //if(currentTick==-1)return 0;
         // baseload = currentTick + ((*it)->zTicks);
          timeBaseLoad=currentTime+ ((*it)->zTime);
          break;
        }
      //}
    }
    double sum=0.0;
    double sum_time=0.0;
    for (std::vector<WorkerInfo*>::iterator it = DLT::workerList.begin() ; it != DLT::workerList.end(); ++it){
       // currentTick =((*it)->eSvcTicks);
        currentTime=((*it)->serviceTime);
       // if(currentTick==-1)return 0;
        //if(((*it)->s)==ON){
          if((((*it)->s)==ON) && (((*it)->serviceTime)>0)) //{
         // if(((*it)->serviceTime)>0)
          {
          //  sum+=((static_cast<double>(baseload))/(((*it)->eSvcTicks)+ ((*it)->zTicks)));
            sum_time+=((static_cast<double>(timeBaseLoad))/(((*it)->serviceTime)+ ((*it)->zTime)));
          }
        //}
    }
  //  double firstload =(1.0/sum);
    double firstloadTime=(1.0/sum_time);
    for (std::vector<WorkerInfo*>::iterator it = DLT::workerList.begin() ; it != DLT::workerList.end(); ++it){
          if((((*it)->s)==ON) && (((*it)->serviceTime)>0)) //{
           // if
          //  { // it is already set to zero for service time 0
            //  (*it)->timeBasedLoad=0.0;
            //  (*it)->alphaLoad=0.0;
           // }
           // else
            {

       //       (*it)->alphaLoad=((static_cast<double>(baseload))/(((*it)->eSvcTicks)+ ((*it)->zTicks)))*firstload;
              (*it)->timeBasedLoad=((static_cast<double>(timeBaseLoad))/(((*it)->serviceTime)+ ((*it)->zTime)))*firstloadTime;
            }
         //}
    }
/* for (std::vector<WorkerInfo*>::iterator it = DLT::workerList.begin() ; it != DLT::workerList.end(); ++it){
            std::cout << (*it)->alphaLoad<< std::endl;
    }*/
    return true;
  }
