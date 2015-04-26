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
 
//provide all required information for the nodes.
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/if_link.h>
#include <iostream>
#include <string.h>
#include <vector>
#include <sys/sysinfo.h>    // sysinfo
#if defined(FASTFLOW_OPENCL)

#include <ff/ocl/OpenCL_Requirement.hpp>

#endif

using namespace std;
#define BUF_MAX 1024
#define MAX_CPU 128

// this is used for DSRI get Hust name server, the one called Display device is redundant and must be merge and remove
struct DeviceInfo{
  DeviceInfo(){}
  int deviceNumber;
  string deviceType;
  string deviceVendor;
  int devicePlatformNumber;
  string devicePlatformName;
  ulong deviceGlobalMemorySize;
  string devicePlatformVendor;
  int SMNumbers;
};


struct nodeInfo{
  nodeInfo(){OpenCL_enable=false; }
  string hostName;
  string interfaceAddress;
  string interfaceNameFamily;
  long numCPU;
  unsigned long totalRam;
  bool OpenCL_enable;
  std::vector<DeviceInfo*> DevicesInfo;

};

/*
int read_fields (FILE *fp, unsigned long long int *fields)
{
  int retval;
  char buffer[BUF_MAX];
 
 
  if (!fgets (buffer, BUF_MAX, fp))
  { perror ("Error"); }
  // line starts with c and a string. This is to handle cpu, cpu[0-9]+ 
  retval = sscanf (buffer, "c%*s %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu",
                            &fields[0], 
                            &fields[1], 
                            &fields[2], 
                            &fields[3], 
                            &fields[4], 
                            &fields[5], 
                            &fields[6], 
                            &fields[7], 
                            &fields[8], 
                            &fields[9]); 
  if (retval == 0)
  { return -1; }
  if (retval < 4) //Atleast 4 fields is to be read 
  {
    fprintf (stderr, "Error reading /proc/stat cpu field\n");
    return 0;
  }
  return 1;
}*/


nodeInfo* get_nodeInfo(){

	char hostname[1024];
	gethostname(hostname, 1024);

  struct ifaddrs *ifaddr, *ifa;
  int family, s, n;
  char host[NI_MAXHOST];
  nodeInfo* node_info=new nodeInfo;

  if (getifaddrs(&ifaddr) == -1) {
     perror("getifaddrs");
     exit(EXIT_FAILURE);
  }

  //Walk through linked list, maintaining head pointer so we can free list later 
  for (ifa = ifaddr, n = 0; ifa != NULL; ifa = ifa->ifa_next, n++) {
      if (ifa->ifa_addr == NULL)
         continue;

      family = ifa->ifa_addr->sa_family;

     // Display interface name and family (including symbolic form of the latter for the common families) 
      if((strcmp (ifa->ifa_name,"wlan0") == 0)&& family == AF_INET ){

     // For an AF_INET* interface address, display the address 
          s = getnameinfo(ifa->ifa_addr,
                 (family == AF_INET) ? sizeof(struct sockaddr_in) :
                                       sizeof(struct sockaddr_in6),
                 host, NI_MAXHOST,
                 NULL, 0, NI_NUMERICHOST);
          if (s != 0) {
             printf("getnameinfo() failed: %s\n", gai_strerror(s));
             exit(EXIT_FAILURE);
          }

          string h_name(hostname);
          node_info->hostName.assign(h_name);
     		string ip_address(host);
     		node_info->interfaceAddress.assign(ip_address);
     		string ip_type("wlan0-AF_INET");
     		node_info->interfaceNameFamily.assign(ip_type);
      } 
  }

  freeifaddrs(ifaddr);


  // total number of cpu cores in the system
  node_info->numCPU=sysconf( _SC_NPROCESSORS_ONLN );
  
  //total node ram size in byte
  struct sysinfo info;
  if (sysinfo(&info) != 0) 
    node_info->totalRam= info.totalram;
  
#if defined(FASTFLOW_OPENCL)

  cl_int status;
  cl_platform_id *platforms = NULL;
  cl_uint numPlatforms;
  node_info->OpenCL_enable=true; 
  status = clGetPlatformIDs(0, NULL, &(numPlatforms));
  platforms =new cl_platform_id[numPlatforms];
  status = clGetPlatformIDs(numPlatforms, platforms,NULL);
  
  for (int i = 0; i<numPlatforms; i++){
    cl_uint numDevices;
    status = clGetDeviceIDs(platforms[i],CL_DEVICE_TYPE_ALL,0,NULL,&(numDevices));
    cl_device_id* deviceIds =new cl_device_id[numDevices];
    // Fill in CLDevice with clGetDeviceIDs()
    status = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL,numDevices,deviceIds,NULL);
      
    for(int j=0; j<numDevices; j++){
      
      DeviceInfo* device_information = new DeviceInfo();
      device_information->devicePlatformNumber=i;
      // estimating max number of thread per device 
      cl_ulong memSize;
      cl_device_type d_type;
      cl_bool b;
      cl_context context;
      cl_int status;
      
      clGetDeviceInfo(deviceIds[j], CL_DEVICE_TYPE, sizeof(cl_device_type), &(d_type), NULL);
      clGetDeviceInfo(deviceIds[j], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong), &(memSize), NULL);
      clGetDeviceInfo(deviceIds[j], CL_DEVICE_AVAILABLE, sizeof(cl_bool), &(b), NULL);
      context = clCreateContext(NULL,1,&deviceIds[j],NULL,NULL,&status);
      
      if((b & CL_TRUE) && (status == CL_SUCCESS)){ 
        device_information->deviceNumber  =j;
        if((d_type)&CL_DEVICE_TYPE_GPU)
          device_information->deviceType="CL_DEVICE_TYPE_GPU";
        else if((d_type)&CL_DEVICE_TYPE_CPU)
          device_information->deviceType="CL_DEVICE_TYPE_CPU";
        else device_information->deviceType="Other";
        
        size_t len;
        status =clGetDeviceInfo(deviceIds[j], CL_DEVICE_VENDOR, 0, NULL, &len);
        //printOCLErrorString(status, std::cout);
        
        char* vendor = new char[len];
        status =clGetDeviceInfo(deviceIds[j], CL_DEVICE_VENDOR, len, vendor, NULL);
        //printOCLErrorString(status, std::cout);
        
        device_information->deviceVendor= vendor;
        status =clGetDeviceInfo(deviceIds[j], CL_DEVICE_PLATFORM, 0, NULL, &len);
        //printOCLErrorString(status, std::cout);
        
        cl_platform_id pId;
        status =clGetDeviceInfo(deviceIds[j], CL_DEVICE_PLATFORM, len, &pId, NULL);
        //printOCLErrorString(status, std::cout);
        
        status =clGetPlatformInfo(pId, CL_PLATFORM_NAME, 0, NULL, &len);
        //printOCLErrorString(status, std::cout);
        
        char* PlatformName = new char[len];
        status =clGetPlatformInfo(pId, CL_PLATFORM_NAME, len, PlatformName, NULL);
        //printOCLErrorString(status, std::cout);
        
        device_information->devicePlatformName= PlatformName;
        status =clGetPlatformInfo(pId, CL_PLATFORM_VENDOR, 0, NULL, &len);
        //printOCLErrorString(status, std::cout);
        
        char* PlatformVendor = new char[len];
        status =clGetPlatformInfo(pId, CL_PLATFORM_VENDOR, len, PlatformVendor, NULL);
        //printOCLErrorString(status, std::cout);
        device_information->devicePlatformVendor= PlatformVendor;
        
        // memory size in byte
        cl_ulong memSize;
        clGetDeviceInfo(deviceIds[j], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong), &(memSize), NULL);
        device_information->deviceGlobalMemorySize=memSize;
        
        // number of SMs
        cl_uint smNo;
        clGetDeviceInfo(deviceIds[j], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &(smNo), NULL);
        device_information->SMNumbers=smNo;

        node_info->DevicesInfo.push_back(device_information);
        clReleaseContext(context);
      }
    }
  }
#endif
return node_info;
}
void PrintNodeInfo (nodeInfo* node_info) {
	
	cout<< "name: "<< node_info->hostName<<endl;
	cout<< "interface Name and Family: "<< node_info->interfaceNameFamily<<endl;
	cout<< "interface Address: "<< node_info->interfaceAddress<<endl;
	cout<< "Number of cores: "<< node_info->numCPU<<endl;
  cout<< "Memory Size: "<< node_info->totalRam<<endl;
  cout<< "OpenCL enable: "<< node_info->OpenCL_enable<<endl;
#if defined(FASTFLOW_OPENCL)
  for(int i=0; i<node_info->DevicesInfo.size();i++){
    cout<< "device Platform Number: "<< node_info->DevicesInfo.at(i)->devicePlatformNumber<<endl;
    cout<< "device Type: "<< node_info->DevicesInfo.at(i)->deviceType<<endl;
    cout<< "device Vendor: "<< node_info->DevicesInfo.at(i)->deviceVendor<<endl;
    cout<< "device number: "<< node_info->DevicesInfo.at(i)->deviceNumber<<endl;
    cout<< " device Platform Name: "<< node_info->DevicesInfo.at(i)->devicePlatformName<<endl;
    cout<< " device Global Memory Size: "<< node_info->DevicesInfo.at(i)->deviceGlobalMemorySize<<endl;
    cout<< "device Platform Vendor: "<< node_info->DevicesInfo.at(i)->devicePlatformVendor<<endl;
  }
#endif
}


