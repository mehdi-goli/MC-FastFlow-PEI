/* -*- Mode: C++; tab-width: 2; c-basic-offset: 4; indent-tabs-mode: nil -*- */

/*!
 * \link
 * \file oclnode.hpp
 * \ingroup streaming_network_arbitrary_shared_memory
 *
 * \brief Defines the OpenCL implementation of FastFlow node
 *
 * This files defines the FastFlow implementation of OpenCL. This
 * implementation enables us to support FastFlow on the GPGPUs. 
 *
 */

/* ***************************************************************************
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published by
 *  the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 *  more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc., 59
 *  Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *  As a special exception, you may use this file as part of a free software
 *  library without restriction.  Specifically, if other files instantiate
 *  templates or use macros or inline functions from this file, or you compile
 *  this file and link it with other files to produce an executable, this file
 *  does not by itself cause the resulting executable to be covered by the GNU
 *  General Public License.  This exception does not however invalidate any
 *  other reasons why the executable file might be covered by the GNU General
 *  Public License.
 *
 ****************************************************************************
Mehdi Goli: goli.mehdi@gmail.com*/
 
#ifndef _FF_OCLNODE_HPP_
#define _FF_OCLNODE_HPP_

#include <ff/node.hpp>
#if defined(FASTFLOW_OPENCL)
#include <ff/PEI/cl_environment.hpp>

#include <map>
#endif
namespace ff{
#if defined(FASTFLOW_OPENCL)

/*!
 * \ingroup streaming_network_arbitrary_shared_memory
 *
 * @{
 */

/*!
 * \class ff_oclNode
 * \ingroup streaming_network_arbitrary_shared_memory
 *
 * \brief OpenCL implementation of FastFlow node
 *
 * This class defines the OpenCL implementation of FastFlow node.
 *
 * This class is defined in \ref ff_oclnode.hpp
 *
 */
class ff_oclNode : public ff_node 
{ 
public:

    /**
     * \brief Setup OCL object
     *
     * It is a pure virtual function. It sets up the OpenCL object of the FastFlow node.
     *
     * \parm id is the identifier of the opencl device
     */ 
    //virtual oclParameter* set_oclParameter(cl_device_id id)=0;

    oclParameter* get_oclParameter()
    {   
        //   std::cout << " switcher val****************************************"<<switcher<<std::endl;
        // std::cout<< " baseclass val****************************************"<<baseclass_ocl_node_deviceId<< std::endl;
        //std::cout << "***************************No dynamicParameters"<<clEnvironment::instance()->dynamicParameters.size()<< std::endl;
        return (clEnvironment::instance()->dynamicParameters.at(baseclass_ocl_node_deviceId));
    };
    /**
     * \brief Releases OCL object
     *
     * It is a pure virtual function, and releases the OpenCL object of the
     * FastFlow node.
     *
     */
    
    /*Mehdi:FF_VIP*/
    virtual void set_actuators(JSONNode actuators)
    {
        if(actuators.type()!=JSON_NULL)
        {
            JSONNode::const_iterator iter = actuators.begin();
            if(strcasecmp(iter->name().c_str(), "Action") == 0)
            {
                if(strcasecmp(iter->as_string().c_str(), "Suspend") == 0 )
                {
                  //  std::cout<<"_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_I am sleeping_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_"<<std::endl;
                    suspend=true;
                }
                else 
                {  
                    std::cout<<"_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_I am resumming OCl_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_"<<std::endl;
                    suspend=false;
                    resume_node();
                }
            }
            else
            {
               JSONNode::const_iterator it =actuators.find_nocase("Device_Number");
                if(it!=actuators.end())
                {
                    switcher = it->as_int();
                    std::cout<< "+++++++++++++++++++++++++++++++++__________switcher_________++++++++++++++++++++++++++++++++++++++++++"<<switcher<<std::endl;
               //device_information->deviceNumber=tmp_int;
                }
                JSONNode::const_iterator jniter = actuators.find_nocase("Status");
                if(jniter!=actuators.end())
                {
                    ff_node::set_status(jniter->as_int());
                }
                got_id_from_server=true;
            }
        }
    }

#if defined(TRACE_FASTFLOW)

    virtual JSONNode get_sensors()
    {
        JSONNode sensors=ff_node::get_sensors();
        int status;         
        //sensors.at_nocase("Component_Name") =abi::__cxa_demangle(typeid(*this).name(), 0,0,&status);
        sensors.at_nocase("Component_Type")="hsequential";
        sensors.push_back(JSONNode("Assigned_Device_Number",switcher));
        sensors.set_name("Hsequential");
        return sensors;
    }
#endif

virtual ~ff_oclNode()
{
     /*Mehdi:FF-VIP*/
    if (end_callback) 
    { 
        end_callback(end_callback_param);
        end_callback = NULL;
    }
}
     
protected:  
    
    /**
     * \brief Constructor
     *
     * It construct the OpenCL node for the device.
     *
     */
    ff_oclNode():baseclass_ocl_node_deviceId(NULL),switcher(-1),got_id_from_server(false)
    {
        first_flag=true; 
        //getAvailableDevices();
    }
    
    /**
     * \brief Device rules
     *
     * It defines the ruls for the device.
     *
     * \parm id is the identifier of the device
     *
     * \return \p true is always returned
     */
   // virtual bool  device_rules(cl_device_id id)
   // {
  //  return true;
  //}
    
    /**
     * TODO
     */
    bool getEXIT()
    {
      if(ff_node::getEXIT()) return true;
      else return false;
    }
    /**
     * \brief Creates OpenCL
     *
     * It initializes OpenCL instance,
     */
    inline void svc_createOCL()
    { 
        if(first_flag)
        {
           /* pthread_mutex_lock(&(mutex_set_policy));
            if(generate_flag)
            {
                generate_flag=false;
                clEnvironment::instance();
                std::cout << "***************************No devices"<<clEnvironment::instance()->clDevices.size()<< std::endl;
                std::cout<< "***************************It has been instantiated once********************"<<std::endl;
                for(std::vector<cl_device_id>::iterator iter=clEnvironment::instance()->clDevices.begin(); iter< clEnvironment::instance()->clDevices.end(); ++iter)
                {
                    clEnvironment::instance()->dynamicParameters[(*iter)]= set_oclParameter((*iter));
                    size_t len;
                    cl_device_type d_type;
                    clGetDeviceInfo((*iter), CL_DEVICE_VENDOR, 0, NULL, &len);
                    char* vendor = new char[len];
                    clGetDeviceInfo((*iter), CL_DEVICE_VENDOR, len, vendor, NULL);
                    printf(" the device vendor is %s\n", vendor);

                    std::cout << "*************************** device"<<(*iter)<< std::endl;
                }
            }*/
            if(got_id_from_server)
            {
                std::cout<<"*******************************************this is the selected Id*******************************************: "<<switcher<<std::endl;
                baseclass_ocl_node_deviceId=clEnvironment::instance()->clDevices.at(switcher);
                cl_device_type dt;
                 clGetDeviceInfo(baseclass_ocl_node_deviceId, CL_DEVICE_TYPE, sizeof(cl_device_type), &(dt), NULL);
                if((dt)&CL_DEVICE_TYPE_GPU)
                {std::cout<<  " the selected device is a gpu device"<< std::endl;}
                if((dt)&CL_DEVICE_TYPE_CPU)  
                {std::cout<< "the selected device is a CPU device"<< std::endl;}
            }
            else
            {
                std::cout<<"*******************************************this is the random selected*******************************************: "<<std::endl;
                if((ff_node::get_my_id())<=2)
                { 
                	baseclass_ocl_node_deviceId=clEnvironment::instance()->clDevices.at(0);
                }
                else
                {	
                	baseclass_ocl_node_deviceId=clEnvironment::instance()->clDevices.at(1);
                }
                //baseclass_ocl_node_deviceId=clEnvironment::instance()->clDevices.at((ff_node::get_my_id()-1)%2);
            }
            first_flag=false;
           // pthread_mutex_unlock(&(mutex_set_policy)); 
        }
        else 
        {
            int i = switcher;
            if(i>=0 && i<clEnvironment::instance()->clDevices.size() && clEnvironment::instance()->clDevices.at(i)!=baseclass_ocl_node_deviceId)
            {
                baseclass_ocl_node_deviceId=clEnvironment::instance()->clDevices.at(i);
                std::cout<<"*******************************************one switch happend Here*******************************"<<std::endl;
               #if defined(TRACE_FASTFLOW)
                ff_node::set_swith_status();
                #endif
            }
        }
    }
    
    /**
     * \brief Releases OpenCL
     *
     * It releases OpenCL instance.
     */
    inline void svc_releaseOCL()
    { 
       
    /*    
    for(std::vector<cl_device_id>::iterator iter=clDevices.begin(); iter< clDevices.end(); ++iter)
      {
        status =clReleaseCommandQueue(clEnvironment::instance()->dynamicParameters.at((*iter))->commandQueue);
          printStatus("Release commandQueue",status);
          status =clReleaseContext(clEnvironment::instance()->dynamicParameters.at((*iter))->context);
          printStatus("Release context",status);
          status =clReleaseProgram(clEnvironment::instance()->dynamicParameters.at((*iter))->program);
          printStatus("Release program",status);
        }*/ 
    }
    
    int getGPUId() const 
    {
        return switcher;
    }
    
private:
    cl_device_id baseclass_ocl_node_deviceId; // is the id which is provided for user
    int switcher;
    int first_flag;
    bool got_id_from_server;
};
#else

class ff_oclNode:public ff_node
{
  ff_oclNode()
  {
    error(" No ONEPCL flag found! Please add -DFASTFLOW_OPENCL in the Compile Command\n");
    exit (EXIT_FAILURE);
  }

};
#endif


/*!
 * @}
 * \endlink
 */
}
#endif
