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
 
#include <ff/PEI/cl_environment.hpp>

/*!
 * \ingroup streaming_network_simple_shared_memory
 *
 * @{
 */

clEnvironment* clEnvironment::m_clEnvironment = NULL;
/**
 * TODO
 */
clEnvironment::clEnvironment()
{
  
#if defined(FASTFLOW_OPENCL)
     //std::cout<<"________________________________________________ Preparing Environment ____________________________________________________________________"<<std::endl;
    cl_int status;
    cl_platform_id *platforms = NULL;
    cl_uint numPlatforms;

    #if defined(FF_GPUCOMPONETS)
        clEnvironment::numGPU=FF_GPUCOMPONETS;
    #else
        clEnvironment::numGPU=10000;
    #endif
    clEnvironment::oclId=0;
    
    status = clGetPlatformIDs(0, NULL, &(numPlatforms));
    platforms =new cl_platform_id[numPlatforms];
    status = clGetPlatformIDs(numPlatforms, platforms,NULL);
    
    for (int i = 0; i<numPlatforms; i++)
    {
        cl_uint numDevices;
        status = clGetDeviceIDs(platforms[i],CL_DEVICE_TYPE_ALL,0,NULL,&(numDevices));
        cl_device_id* deviceIds =new cl_device_id[numDevices];
        // Fill in CLDevice with clGetDeviceIDs()
        status = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL,numDevices,deviceIds,NULL);
        
        for(int j=0; j<numDevices; j++)
        {
            // estimating max number of thread per device 
            cl_bool b;
            cl_context context;
            cl_int status;
            cl_device_type dt;

          
            clGetDeviceInfo(deviceIds[j], CL_DEVICE_AVAILABLE, sizeof(cl_bool), &(b), NULL);
            context = clCreateContext(NULL,1,&deviceIds[j],NULL,NULL,&status);
            clGetDeviceInfo(deviceIds[j], CL_DEVICE_TYPE, sizeof(cl_device_type), &(dt), NULL);
            if((dt)&CL_DEVICE_TYPE_GPU)
                {std::cout<< j << " is a gpu device"<< std::endl;}
            else if((dt)&CL_DEVICE_TYPE_CPU)  
                {std::cout<< j<< " is a CPU device"<< std::endl;}
            if((b & CL_TRUE) && (status == CL_SUCCESS))
            {
          
                clDevices.push_back(deviceIds[j]);
            }
            clReleaseContext(context);
        }
    }
#endif
}

int clEnvironment::getOCLID()
{
    int id;
    pthread_mutex_lock(&mutex_getOCLID);
    id=oclId++;
    pthread_mutex_unlock(&mutex_getOCLID);
    return id;

}
int clEnvironment::getGPUDevice()
{
    cl_device_type dt;
    for(int i=0; i<clDevices.size(); i++)
    {
        clGetDeviceInfo(clDevices[i], CL_DEVICE_TYPE, sizeof(cl_device_type), &(dt), NULL);
        if((dt)&CL_DEVICE_TYPE_GPU) return i;
    }
    return -1;
}

int clEnvironment::getCPUDevice()
{
    cl_device_type dt;
    for(int i=0; i<clDevices.size(); i++)
    {
        clGetDeviceInfo(clDevices[i], CL_DEVICE_TYPE, sizeof(cl_device_type), &(dt), NULL);
        if((dt)&CL_DEVICE_TYPE_CPU) return i;
    }
    return -1;
}

void clEnvironment::cleanall()
{
     cl_int status;
     std::cout<< "**********************cleanning**********"<<clEnvironment::instance()->clDevices.size()<<std::endl;

    for(std::vector<cl_device_id>::iterator iter=clEnvironment::instance()->clDevices.begin(); iter< clEnvironment::instance()->clDevices.end(); ++iter)
    {
        status =clReleaseCommandQueue(clEnvironment::instance()->dynamicParameters.at((*iter))->commandQueue);
        printStatus("Release commandQueue",status);
        status =clReleaseContext(clEnvironment::instance()->dynamicParameters.at((*iter))->context);
        printStatus("Release context",status);
        status =clReleaseProgram(clEnvironment::instance()->dynamicParameters.at((*iter))->program);
        printStatus("Release program",status);
    } 
}
#if defined(FASTFLOW_OPENCL)
void clEnvironment::prepare(ff_oclParam* oclparam)
{

        if(generate_flag)
            {
                generate_flag=false;
            //    clEnvironment::instance();
             //   std::cout << "***************************No devices"<<clEnvironment::instance()->clDevices.size()<< std::endl;
              //  std::cout<< "***************************It has been instantiated once********************"<<std::endl;
                for(std::vector<cl_device_id>::iterator iter=clEnvironment::instance()->clDevices.begin(); iter< clEnvironment::instance()->clDevices.end(); ++iter)
                {
                    clEnvironment::instance()->dynamicParameters[(*iter)]= oclparam->set_oclParameter((*iter));
                    //size_t len;
                    //cl_device_type d_type;
                    //clGetDeviceInfo((*iter), CL_DEVICE_VENDOR, 0, NULL, &len);
                    //char* vendor = new char[len];
                    //clGetDeviceInfo((*iter), CL_DEVICE_VENDOR, len, vendor, NULL);
                   // printf(" the device vendor is %s\n", vendor);

                //    std::cout << "*************************** device"<<(*iter)<< std::endl;
                }
            }
    //}

}
#endif
  
/**
 * TODO
 */
clEnvironment * clEnvironment::instance()
{ 
    if (!m_clEnvironment)
    {
        pthread_mutex_lock(&instanceMutex);
        if (!m_clEnvironment)
            m_clEnvironment = new clEnvironment();
        pthread_mutex_unlock(&instanceMutex);
    }
    else return m_clEnvironment; 
}