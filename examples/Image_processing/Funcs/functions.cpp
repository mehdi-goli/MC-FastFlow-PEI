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
 Mehdi Goli: m.goli@rgu.ac.uk*/

#include <ff/ocl/OpenCL_Requirement.hpp>
#include "/users/mehdi/Thesis/fastflow-2.0.2-paraphrase/examples/Image_processing/Funcs/SDKBitMap.hpp"
#include <malloc.h>
#include <iostream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <vector>
#include <fstream>
#include <cmath>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cerrno>
#include <strings.h>

extern "C" {
using namespace std;

int quiet=1;
#define IMAGE_WIDTH 256
#define IMAGE_HEIGHT 256
#define MASK_WIDTH 27
#define MASK_HEIGHT 27
int max_strlen=6144;
int folder_size=1024;
int image_folder_loc=1;
int image_count=0;
#define ITER_CONV 2
struct task_t
{
    task_t()
    {
        iter=ITER_CONV;
        i_w=image_folder_loc*IMAGE_WIDTH;
        i_h=image_folder_loc*IMAGE_HEIGHT;
        m_w=MASK_WIDTH;
        m_h=MASK_HEIGHT;
    }
    cl_uint* inpt;
    cl_uint* outpt;
    cl_float* msk;
    cl_uint seq_number;
    int iter;
    cl_uint i_w;
    cl_uint i_h;
    cl_uint m_w;
    cl_uint m_h;
};

cl_uint * benchmark_inpt;
cl_float* benchmark_msk;

#define FACTOR 60
#define ITER 10 // changg the iteration will damage the image
int cnt=1; // for different directory
int input_size=7;
// to read from directory
char * dir;
DIR *dp;
struct dirent *dirp;
// flag to read directory only once
bool set_directory=true;
bool close_dir=false;
string out_folder;
int img_count=0;
SDKBitMap*  benchmarck_bitmap;
bool firsFile=true;
int num_file_in_folder=1024;

/**
* Custom type for gaussian parameters
* precomputation
*/
typedef struct _GaussParms
{
    float nsigma;
    float alpha;
    float ema;
    float ema2;
    float b1;
    float b2;
    float a0;
    float a1;
    float a2;
    float a3;
    float coefp;
    float coefn;
} GaussParms, *pGaussParms;

struct image_task{
    image_task(std::string input_path, std::string output_path){
        outputImageName= output_path;
        factor=FACTOR;
        iter=ITER;
        gParam= new GaussParms();
        // get width and height of input image
        height = benchmarck_bitmap->getHeight();
        width = benchmarck_bitmap->getWidth();
        // allocate memory for input & output image data
        inputImageData  = new cl_uchar4[width*height]();
        // allocate memory for output image data
        outputImageData = new cl_uchar4[width*height]();
         // error check
        if(benchmarck_bitmap->getPixels() == NULL)
        {
            printf("Failed to read pixel Data!\n");
            exit(EXIT_FAILURE);
        }

        // Copy pixel data into inputImageData
        memcpy(inputImageData, benchmarck_bitmap->getPixels(), width * height * sizeof(cl_uchar4));
    };
    ~image_task()
    {
       
        delete  gParam;
        gParam=NULL;
       // delete inputBitmap;
        //inputBitmap=NULL;
        delete [] inputImageData;
        inputImageData=NULL;
        delete [] outputImageData;
        outputImageData=NULL;
        outputImageName.clear();
    };
   // SDKBitMap*  inputBitmap;
    GaussParms* gParam;
    std::string outputImageName;
    cl_uchar4*  inputImageData;              //< Input bitmap data to device 
    cl_uchar4*  outputImageData;             //< Output from device 
    cl_uint     width;                          //< Width of image 
    cl_uint     height;                         //< Height of image 
    int         factor;
    int         iter; 
};

int fillRandom(cl_uint * arrayPtr,  const int width,const int height,const cl_uint rangeMin,const cl_uint rangeMax,unsigned int seed)
{
    if(!arrayPtr)
    {
        printf("Cannot fill array. NULL pointer.");
        return 0;
    }

    if(!seed)
        seed = (unsigned int)time(NULL);

    srand(seed);
    double range = double(rangeMax - rangeMin) + 1.0; 

    /* random initialisation of input */
    for(int i = 0; i < height; i++)
        for(int j = 0; j < width; j++)
        {
            int index = i*width + j;
            arrayPtr[index] = rangeMin + (cl_uint)(range*rand()/(RAND_MAX + 1.0)); 
        }

    return 1;
};

// setup ocl parameter
oclParameter*  set_oclParameters(cl_device_id dId, std::string kernelPath)
{
    /* create a CL program using the kernel source */
    //std::string kernelPath = "/users/mehdi/Thesis/fastflow-2.0.2.10-VIP/examples/simple-convolution/H_Version/SimpleConvolution_Kernels.cl";
    std::ifstream t(kernelPath.c_str());
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::string source_string = buffer.str() + '\0';
    const char* source =source_string.c_str();                                                                                                                 

    oclParameter* oclParams = new oclParameter(dId);
    
    if(!quiet)
        std::cout<< dId<< std::endl;
    
    cl_int status;
    // creating the context
    oclParams->context = clCreateContext(NULL,1,&dId,NULL,NULL,&status);
    if(!quiet)
        printStatus("CreatContext: ", status);
 
    size_t sourceSize[] = { strlen(source) };
    if(!quiet)
        std::cout << sourceSize[0] << " bytes in source" << std::endl;

    oclParams->program = clCreateProgramWithSource(oclParams->context,1,(const char **)&source,sourceSize,&status);
    if(!quiet)
        printStatus("CreateProgramWithSource: ", status);

    status = clBuildProgram(oclParams->program,1,&dId,NULL,NULL,NULL);

    size_t len;
    clGetProgramBuildInfo(oclParams->program, dId, CL_PROGRAM_BUILD_LOG, 0, NULL, &len);

    char* buff = new char[len];
    clGetProgramBuildInfo(oclParams->program, dId, CL_PROGRAM_BUILD_LOG, len, buff, NULL);
    if(!quiet)
        printf("%s\n", buff);

    //creating command quue
    if(!quiet)
        printStatus("BuildProgram",status);

    /* The block is to move the declaration of prop closer to its use */
    cl_command_queue_properties prop = 0;
    oclParams->commandQueue = clCreateCommandQueue(oclParams->context, oclParams->d_id, prop, &status);
    if(!quiet)
        printStatus("CreateCommandQueue",status);
    
    if(!quiet)
        printf("run per worker per device\n");

    return oclParams;
  };

void * execute_kernel_gpu(void * task, oclParameter* oclParams) 
{

    if (task==NULL)
    { 
        printf("null\n"); return NULL;
    }
    else
    {
        task_t *t =(task_t*)task;
        t->outpt=new cl_uint[(t->i_w)*(t->i_h)];//outpt;
        cl_int status; 
        /* get a kernel object handle for a kernel with the given name */
        cl_kernel kernel = clCreateKernel(oclParams->program, "simpleConvolution", &status);
        if(!quiet) 
            printStatus("CreateKernel",status);

        cl_mem inputBuffer =(clCreateBuffer(oclParams->context, CL_MEM_READ_WRITE , sizeof(cl_uint ) * (t->i_w) * (t->i_h), NULL, &status));

        if(!quiet) 
            printStatus("createbuf1",status);

        cl_mem outputBuffer =(clCreateBuffer(oclParams->context, CL_MEM_READ_WRITE, sizeof(cl_uint ) * (t->i_w) * (t->i_h),NULL, &status));
  
        if(!quiet) 
          printStatus("createbuf2",status);

        cl_mem maskBuffer =(clCreateBuffer(oclParams->context, CL_MEM_READ_WRITE , sizeof(cl_float ) * (t->m_w) * (t->m_h),NULL, &status));
        if(!quiet) 
            printStatus("createbuf3",status);

        // Use clEnqueueWriteBuffer() to write input array A to the device buffer bufferA
        status = clEnqueueWriteBuffer(oclParams->commandQueue,inputBuffer,CL_FALSE,0,sizeof(cl_uint ) * (t->i_w) * (t->i_h), t->inpt,0,NULL,NULL);
        if(!quiet)    
            printStatus("rwritebuffe_inputr",status);              
      
        // Use clEnqueueWriteBuffer() to write input array B to the device buffer bufferB
        status = clEnqueueWriteBuffer(oclParams->commandQueue,maskBuffer,CL_FALSE,0,sizeof(cl_float) * (t->m_w) * (t->m_h),t->msk,0,NULL,NULL); 
        if(!quiet)      
            printStatus("rwritebuffe_inputr",status);     
      
         /* Check group size against kernelWorkGroupSize */
        size_t kernelWorkGroupSize;
        status = clGetKernelWorkGroupInfo(kernel, oclParams->d_id,CL_KERNEL_WORK_GROUP_SIZE,sizeof(size_t),&kernelWorkGroupSize,0);
        if(!quiet) 
            printStatus("kernel grupsize match checking",status);
     
        size_t blockSizeX=256;
        if(blockSizeX>kernelWorkGroupSize) blockSizeX=kernelWorkGroupSize;
        if(blockSizeX>(t->i_w)) blockSizeX=(t->i_w);
        cl_event events[2];
        size_t globalThreads[1];
        size_t localThreads[1];
        globalThreads[0] = ((t->i_w)*(t->i_h));
        localThreads[0]  =blockSizeX;
        int x_w=0;
      
       /*** Set appropriate arguments to the kernel ***/
        status = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&(outputBuffer));
        if(!quiet) 
            printStatus("setKernel1",status);

        status = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&(inputBuffer));
        if(!quiet)
            printStatus("setKernel2",status);

        status = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&(maskBuffer));
        if(!quiet)
            printStatus("setKernel3",status);

        cl_uint2 inputDimensions = {(t->i_w)    , (t->i_h)};
        status = clSetKernelArg(kernel, 3, sizeof(cl_uint2), (void *)&inputDimensions);
        if(!quiet)
            printStatus("setKernel4",status);

        cl_uint2 maskDimensions  = {(t->m_w), t->m_h};
        status = clSetKernelArg(kernel, 4,  sizeof(cl_uint2), (void *)&maskDimensions);
        if(!quiet) 
            printStatus("setKernel5",status);

        // image extended width
        status = clSetKernelArg(kernel,5, sizeof(cl_int), &(x_w));
        if(!quiet) 
            printStatus("setKernel6",status);

        /* Enqueue a kernel run call.*/
        status = clEnqueueNDRangeKernel(oclParams->commandQueue,kernel,1,NULL,globalThreads,localThreads,0,NULL,&events[0]);     
        if(!quiet)      
            printStatus("enqueueNDRangeKernel",status);

        /* wait for the kernel call to finish execution */
        status = clWaitForEvents(1, &events[0]);
        if(!quiet)      
            printStatus("events_0",status);   


        ///from here
        for(int i=0; i<t->iter; ++i)
        {
            //std::cout <<"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&Here is your error"<<std::endl;
            events[0]=0;       
            // output : input Buffer
            status = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&(inputBuffer));
            if(!quiet)
                printStatus("setKernel1",status);
            // input : input buffer image
            status = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&(outputBuffer));
            if(!quiet) 
                printStatus("setKernel2",status);

            status = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&(maskBuffer));
            if(!quiet) 
                printStatus("setKernel3",status);

            status = clSetKernelArg(kernel, 3, sizeof(cl_uint2), (void *)&inputDimensions);
            if(!quiet) 
                printStatus("setKernel4",status);

            status = clSetKernelArg(kernel, 4,  sizeof(cl_uint2), (void *)&maskDimensions);
            if(!quiet) 
                printStatus("setKernel5",status);
            // image extended width
            status = clSetKernelArg(kernel,5, sizeof(cl_int), &(x_w));
            if(!quiet) 
                printStatus("setKernel6",status);

            // Enqueue a kernel run call.
            status = clEnqueueNDRangeKernel(oclParams->commandQueue,kernel,1,NULL,globalThreads,localThreads,0,NULL,&events[0]); 
            if(!quiet) 
                printStatus("enqueueNDRangeKernel",status);
            
            clFlush(oclParams->commandQueue);
            
            //wait for the kernel call to finish execution 
            status = clWaitForEvents(1, &events[0]);
            if(!quiet) 
                printStatus("events_0",status); 
            
            status = clReleaseEvent(events[0]);
            if(!quiet) 
                printStatus("release_events_0",status); 

            // third time to get the write one
            // output:output
            events[0]=0;
            status = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&(outputBuffer));
            if(!quiet) 
                printStatus("setKernel1",status);
            //input:input
            status = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&(inputBuffer));
            if(!quiet) 
                printStatus("setKernel2",status);

            status = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&(maskBuffer));
            if(!quiet) 
                printStatus("setKernel3",status);

            status = clSetKernelArg(kernel, 3, sizeof(cl_uint2), (void *)&inputDimensions);
            if(!quiet) 
                printStatus("setKernel4",status);

            status = clSetKernelArg(kernel, 4,  sizeof(cl_uint2), (void *)&maskDimensions);
            if(!quiet) 
                printStatus("setKernel5",status);
            // image extended width
            status = clSetKernelArg(kernel,5, sizeof(cl_int), &(x_w));
            if(!quiet) 
                printStatus("setKernel6",status);

            // Enqueue a kernel run call.
            status = clEnqueueNDRangeKernel(oclParams->commandQueue,kernel,1,NULL,globalThreads,localThreads,0,NULL,&events[0]);     
            if(!quiet) 
                printStatus("enqueueNDRangeKernel",status);
            clFlush(oclParams->commandQueue);
            // wait for the kernel call to finish execution 
            status = clWaitForEvents(1, &events[0]);
            if(!quiet) 
                printStatus("events_0",status); 
            status = clReleaseEvent(events[0]);
            if(!quiet) 
                printStatus("release_events_0",status); 
        }
        //til here

        /* Enqueue readBuffer*/
        status = clEnqueueReadBuffer(oclParams->commandQueue,outputBuffer,CL_TRUE, 0,(t->i_w) * (t->i_h) * sizeof(cl_uint),t->outpt,0,NULL,&events[1]); 
        if(!quiet)      printStatus("runbuffer",status); 
        
        size_t len;
        //cl_device_type d_type;
        clGetDeviceInfo(oclParams->d_id, CL_DEVICE_VENDOR, 0, NULL, &len);
        char* vendor = new char[len];
        clGetDeviceInfo(oclParams->d_id, CL_DEVICE_VENDOR, len, vendor, NULL);
        if(!quiet)      printf(" the device vendor is %s\n", vendor);    
        /* Wait for the read buffer to finish execution */
        status = clWaitForEvents(1, &events[1]);
        if(!quiet)      printStatus("event_1",status); 
        status = clReleaseEvent(events[1]); 
        if(!quiet)      printStatus("release_events_0",status);
          
        if(!quiet) 
            printf("GPU done the job!\n"); 

        clReleaseMemObject(inputBuffer);
        inputBuffer=NULL;

        clReleaseMemObject(outputBuffer);
        outputBuffer=NULL;

        clReleaseMemObject(maskBuffer);
        maskBuffer=NULL;

        task=t;

        return task;
    }
};




void * execute_kernel_cpu(void * task, oclParameter* oclParams) 
{
    if (task==NULL)
    { 
        printf("null\n"); 
        return NULL;
    }
    else
    {
        task_t *t =(task_t*)task;
        t->outpt=new cl_uint[(t->i_w)*(t->i_h)];//outpt;
        cl_int status; 
        /* get a kernel object handle for a kernel with the given name */
        cl_kernel kernel = clCreateKernel(oclParams->program, "simpleConvolution", &status);
        if(!quiet) 
            printStatus("CreateKernel",status);

        cl_mem inputBuffer =(clCreateBuffer(oclParams->context, CL_MEM_READ_WRITE , sizeof(cl_uint ) * (t->i_w) * (t->i_h), NULL, &status));

        if(!quiet) 
            printStatus("createbuf1",status);

        cl_mem outputBuffer =(clCreateBuffer(oclParams->context, CL_MEM_READ_WRITE, sizeof(cl_uint ) * (t->i_w) * (t->i_h),NULL, &status));
  
        if(!quiet) 
          printStatus("createbuf2",status);

        cl_mem maskBuffer =(clCreateBuffer(oclParams->context, CL_MEM_READ_WRITE , sizeof(cl_float ) * (t->m_w) * (t->m_h),NULL, &status));
        if(!quiet) 
            printStatus("createbuf3",status);

         // Use clEnqueueWriteBuffer() to write input array A to the device buffer bufferA
        status = clEnqueueWriteBuffer(oclParams->commandQueue,inputBuffer,CL_FALSE,0,sizeof(cl_uint ) * (t->i_w) * (t->i_h), t->inpt,0,NULL,NULL);
        if(!quiet)    
            printStatus("rwritebuffe_inputr",status);              
      
        // Use clEnqueueWriteBuffer() to write input array B to the device buffer bufferB
        status = clEnqueueWriteBuffer(oclParams->commandQueue,maskBuffer,CL_FALSE,0,sizeof(cl_float) * (t->m_w) * (t->m_h),t->msk,0,NULL,NULL); 
        if(!quiet)      
            printStatus("rwritebuffe_inputr",status); 

         /* Check group size against kernelWorkGroupSize */
        size_t kernelWorkGroupSize;
        cl_event events[2];
        size_t globalThreads[1];
        size_t localThreads[1];
        cl_uint2 inputDimensions = {(t->i_w)    , (t->i_h)};
        cl_uint2 maskDimensions  = {(t->m_w), t->m_h};
        size_t blockSizeX;
        
        status = clGetKernelWorkGroupInfo(kernel, oclParams->d_id,CL_KERNEL_WORK_GROUP_SIZE,sizeof(size_t),&kernelWorkGroupSize,0);
        if(!quiet) 
            printStatus("kernel grupsize match checking",status);             
        
        blockSizeX= kernelWorkGroupSize;
        if (blockSizeX>((t->i_w)* (t->i_h))) 
        {
            blockSizeX=((t->i_w)* (t->i_h));
            //globalThreads[0]=((t->i_w)* (t->i_h));
        }
        localThreads[0]  =blockSizeX;
        globalThreads[0] = blockSizeX*8;
        if(globalThreads[0]>((t->i_w)* (t->i_h)))
        {
             globalThreads[0]=((t->i_w)* (t->i_h));
        }

        int x_w;
        int check_range_x = (((t->i_w)* (t->i_h))/(globalThreads[0]));
        //std::cout << "check_range_x:" << check_range_x<< std::endl;
       // std::cout<<  "globalThreads:"<<globalThreads[0]<< std::endl;
       // std::cout<< "localThreads:"<< localThreads[0]<< std::endl;

        for(int k=0; k<check_range_x; ++k)
        {
            x_w= k*(globalThreads[0]);
            /*** Set appropriate arguments to the kernel ***/
            status = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&(outputBuffer));
            if(!quiet) 
                printStatus("setKernel1",status);

            status = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&(inputBuffer));
            if(!quiet)
                printStatus("setKernel2",status);

            status = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&(maskBuffer));
            if(!quiet)
                printStatus("setKernel3",status);

            status = clSetKernelArg(kernel, 3, sizeof(cl_uint2), (void *)&inputDimensions);
            if(!quiet)
                printStatus("setKernel4",status);

            status = clSetKernelArg(kernel, 4,  sizeof(cl_uint2), (void *)&maskDimensions);
            if(!quiet) 
                printStatus("setKernel5",status);
            // image extended width
            status = clSetKernelArg(kernel,5, sizeof(cl_int), &(x_w));
            if(!quiet) 
                printStatus("setKernel6",status);


            /* Enqueue a kernel run call.*/
            status = clEnqueueNDRangeKernel(oclParams->commandQueue,kernel,1,NULL,globalThreads,localThreads,0,NULL,&events[0]);     
            if(!quiet)      
                printStatus("enqueueNDRangeKernel",status);

            /* wait for the kernel call to finish execution */
            status = clWaitForEvents(1, &events[0]);
            if(!quiet)      
                printStatus("events_0",status);   
        }

        ///from here
        for(int i=0; i<t->iter; ++i)
        {
            for(int k=0; k<check_range_x; ++k)
            {
                x_w= k*(globalThreads[0]);
                //std::cout <<"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&Here is your error"<<std::endl;
                events[0]=0;       
                // output : input Buffer
                status = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&(inputBuffer));
                if(!quiet)
                    printStatus("setKernel1",status);
                // input : input buffer image
                status = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&(outputBuffer));
                if(!quiet) 
                    printStatus("setKernel2",status);

                status = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&(maskBuffer));
                if(!quiet) 
                    printStatus("setKernel3",status);

                status = clSetKernelArg(kernel, 3, sizeof(cl_uint2), (void *)&inputDimensions);
                if(!quiet) 
                    printStatus("setKernel4",status);

                status = clSetKernelArg(kernel, 4,  sizeof(cl_uint2), (void *)&maskDimensions);
                if(!quiet) 
                    printStatus("setKernel5",status);

                // image extended width
                status = clSetKernelArg(kernel,5, sizeof(cl_int), &(x_w));
                if(!quiet) 
                    printStatus("setKernel6",status);


                // Enqueue a kernel run call.
                status = clEnqueueNDRangeKernel(oclParams->commandQueue,kernel,1,NULL,globalThreads,localThreads,0,NULL,&events[0]); 
                if(!quiet) 
                    printStatus("enqueueNDRangeKernel",status);
                
                clFlush(oclParams->commandQueue);
                
                //wait for the kernel call to finish execution 
                status = clWaitForEvents(1, &events[0]);
                if(!quiet) 
                    printStatus("events_0",status);

                status = clReleaseEvent(events[0]);
                if(!quiet) 
                    printStatus("release_events_0",status); 
            }
            // third time to get the write one
            // output:output
            for(int k=0; k<check_range_x; ++k)
            {
                x_w= k*(globalThreads[0]);
                events[0]=0;
                status = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&(outputBuffer));
                if(!quiet) 
                    printStatus("setKernel1",status);
                //input:input
                status = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&(inputBuffer));
                if(!quiet) 
                    printStatus("setKernel2",status);

                status = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&(maskBuffer));
                if(!quiet) 
                    printStatus("setKernel3",status);

                status = clSetKernelArg(kernel, 3, sizeof(cl_uint2), (void *)&inputDimensions);
                if(!quiet) 
                    printStatus("setKernel4",status);

                status = clSetKernelArg(kernel, 4,  sizeof(cl_uint2), (void *)&maskDimensions);
                if(!quiet) 
                    printStatus("setKernel5",status);
                // image extended width
                status = clSetKernelArg(kernel,5, sizeof(cl_int), &(x_w));
                if(!quiet) 
                    printStatus("setKernel6",status);

           
                // Enqueue a kernel run call.
                status = clEnqueueNDRangeKernel(oclParams->commandQueue,kernel,1,NULL,globalThreads,localThreads,0,NULL,&events[0]);     
                if(!quiet) 
                    printStatus("enqueueNDRangeKernel",status);
                
                clFlush(oclParams->commandQueue);
                
                // wait for the kernel call to finish execution 
                status = clWaitForEvents(1, &events[0]);
                if(!quiet) 
                    printStatus("events_0",status); 
                
                status = clReleaseEvent(events[0]);
                if(!quiet) 
                    printStatus("release_events_0",status); 
            }
        }
////til here


        /* Enqueue readBuffer*/
        status = clEnqueueReadBuffer(oclParams->commandQueue,outputBuffer,CL_TRUE, 0,(t->i_w) * (t->i_h) * sizeof(cl_uint),t->outpt,0,NULL,&events[1]); 
        if(!quiet)      
            printStatus("runbuffer",status); 
        
        size_t len;
        clGetDeviceInfo(oclParams->d_id, CL_DEVICE_VENDOR, 0, NULL, &len);

        char* vendor = new char[len];
        clGetDeviceInfo(oclParams->d_id, CL_DEVICE_VENDOR, len, vendor, NULL);
        if(!quiet)      
            printf(" the device vendor is %s\n", vendor);    
        /* Wait for the read buffer to finish execution */
        status = clWaitForEvents(1, &events[1]);
        if(!quiet)     
            printStatus("event_1",status); 
        status = clReleaseEvent(events[1]); 
        if(!quiet)      
            printStatus("release_events_0",status);
          
        if(!quiet) 
            printf("GPU done the job!\n"); 

        clReleaseMemObject(inputBuffer);
        inputBuffer=NULL;

        clReleaseMemObject(outputBuffer);
        outputBuffer=NULL;

        clReleaseMemObject(maskBuffer);
        maskBuffer=NULL;

        task=t;

        return task;
    }
};


void* first_stage_emitter_function(void *) {
  int* t;
    if(max_strlen>=0){
      --max_strlen;
      int* t = ( int*)malloc(sizeof(int));
      if (!t) abort();
      *t=max_strlen;
      return ((void*)(t)); 
    }else return NULL;
};

void* first_stage_worker_function(void*){
   // waitSignal();
  //  if(!cnt_task) 
  //      return NULL;

    int* t =( int*)malloc(sizeof(int));//(int*) cnt_task; 
    *t= ((max_strlen)>0? max_strlen:-1); 
    --max_strlen;
    task_t*  im_task=NULL;
    
    if(!quiet)    
        std::cout<< "the global task id is :" << *t<< std::endl;
    
    if(*t!=-1)
    {
        
        if(!(benchmark_msk))
        {
           // std::cout << "*****************Generating first Image*************88"<< std::endl;
            benchmark_inpt= new cl_uint[image_folder_loc*IMAGE_WIDTH * image_folder_loc*IMAGE_HEIGHT]();
           // std::cout << "*****************new bench marck created*************88"<< std::endl;
            fillRandom(benchmark_inpt, image_folder_loc*IMAGE_WIDTH, image_folder_loc*IMAGE_HEIGHT, 0, 255,0); 
           // std::cout << "*****************new bench mark filled*************88"<< std::endl;
            benchmark_msk= new cl_float[MASK_WIDTH*MASK_HEIGHT]();
           // std::cout << "*****************new mask created*************88"<< std::endl;
            //for(int i=0; i < ((MASK_WIDTH)*(MASK_HEIGHT)); ++i) benchmark_msk[i] = 0;
            //memset(benchmark_msk, 0, MASK_WIDTH*MASK_HEIGHT * sizeof(cl_float));
            //std::cout << "*****************new mask created*************88"<< std::endl;

            
            float val = 1.0f/((MASK_WIDTH) * 2.0f - 1.0f);
          
            for(int i=0; i < (MASK_WIDTH); i++)
            {
                int y = (MASK_HEIGHT)/2;
                benchmark_msk[y*(MASK_WIDTH) + i] = val;
            }
        
            for(int i=0; i < (MASK_HEIGHT); i++)
            {
                int x = (MASK_WIDTH)/2;
                benchmark_msk[i*(MASK_WIDTH) + x] = val;
            }  
            //std::cout << "*****************new mask width filled*************88"<< std::endl;       
        }

        if(((*t)!=0)&&((*t)!=folder_size)&& ((*t)%folder_size==0))
        {
            image_folder_loc++;
            benchmark_inpt= new cl_uint[ image_folder_loc*IMAGE_WIDTH * image_folder_loc*IMAGE_HEIGHT];
            fillRandom(benchmark_inpt, image_folder_loc*IMAGE_WIDTH, image_folder_loc*IMAGE_HEIGHT, 0, 255,0);
        }

           // for(int i=0; i < ((im_task->m_w)*(im_task->m_h)); i++) im_task->msk[i] = 0;
            
           // float val = 1.0f/((im_task->m_w) * 2.0f - 1.0f);
          
        //    for(int i=0; i < (im_task->m_w); i++)
        //    {
          //      int y = (im_task->m_h)/2;
           //     im_task->msk[y*(im_task->m_w) + i] = val;
           // }
        
            //for(int i=0; i < (im_task->m_h); i++)
           // {
            //    int x = (im_task->m_w)/2;
             //   im_task->msk[i*(im_task->m_w) + x] = val;
            //}


        im_task = new task_t;
        im_task->seq_number=*t;

        im_task->inpt= new cl_uint[ (im_task->i_w) * (im_task->i_h)];
        memcpy(im_task->inpt, benchmark_inpt, (im_task->i_w) * (im_task->i_h) * sizeof(cl_uint));
       
        im_task->msk = new cl_float[(im_task->m_w) * (im_task->m_h)];
        memcpy(im_task->msk, benchmark_msk, (im_task->m_w) * (im_task->m_h) * sizeof(cl_float));
        ++image_count;
        if(!quiet)
        	std::cout<< "this is the generated task sofar:" << image_count<< std::endl;       
        //}
    }
       
    delete [] t;
    t=NULL;
   // cnt_task=NULL;
    return (void*)im_task;
};

void* first_stage_collector_function(void* im_task){
    return im_task;
};

void* iterative_stage_emitter_function(void * im_task) {
  if(im_task!=NULL){
      return((void*)(im_task)); // to use static scheduler it must be ff_send_out otherewise it does not work
    }else return NULL;
}; 

void* iterative_stage_collector_function(void* im_task){
    return im_task;
};

void* last_stage_function(void* im_task){
    //count_before++;
    if(im_task!=NULL){
    //  count_after++;
        task_t* t = (task_t*)im_task;
        if(!quiet)        std::cout<< "task number :" << ((task_t*)im_task)->seq_number<< " has been received"<<std::endl;
        delete [] t->inpt;
        t->inpt=NULL;
        delete [] t->outpt;
        t->outpt=NULL;
        delete [] t->msk;
        t->msk=NULL;
        delete [] t;
        t=NULL;
        im_task=NULL;
      //  wakeSignal();
        int* i = new int(1);
      return (void*)i;
    }
    else return NULL;
};


void * execute_kernel(void * task, oclParameter* oclParams) {
    
    if (task==NULL)
    { 
        printf("null\n"); return NULL;
    }
    else
    {
        cl_device_type d_type;
        clGetDeviceInfo(oclParams->d_id, CL_DEVICE_TYPE, sizeof(cl_device_type), &(d_type), NULL);
        if((d_type)&CL_DEVICE_TYPE_CPU)
        {
            return execute_kernel_cpu(task, oclParams);
        }
        else
        {
         return execute_kernel_gpu(task, oclParams);   
        }
    }
};



void* write_benchmarck(void* img_task)
{
    image_task * img_tsk = (image_task*) img_task;
    delete  img_tsk;
    img_tsk=NULL;
    img_task=NULL;
    int *t =new int(1);
    return (void*)t;
};
void* read_benchmarck(void* )
{
    std::string tmp("/dev/null");
    std::string filepath;
    
    if(cnt< input_size)
    {
        if(set_directory)
        {
            const char* str1;
            if(cnt==1) 
               str1="/users/mehdi/Thesis/Input_image/sixVersion";
            else if(cnt==2) 
               str1="/users/mehdi/Thesis/Input_image/forVersion";
            else if(cnt==3)
               str1="/users/mehdi/Thesis/Input_image/thiVersion";
           else if (cnt==4) 
               str1="/users/mehdi/Thesis/Input_image/secVersion";
            else if(cnt==5) 
               str1="/users/mehdi/Thesis/Input_image/fifVersion";
            else if(cnt==6)
                str1="/users/mehdi/Thesis/Input_image/eitVersion";
            std::string str(str1);
            str += "/im_001.bmp";
            filepath=str;   
            set_directory=false;
            std::cout<< "*************file"<<str<< std::endl;
        
            if(benchmarck_bitmap) 
            {
                delete benchmarck_bitmap;
                benchmarck_bitmap=NULL;
            }
        
            benchmarck_bitmap= new SDKBitMap();
            benchmarck_bitmap->load(str.c_str());
        
            if(!(benchmarck_bitmap->isLoaded()))
            {
                printf("Failed to load input image!\n");
                exit(EXIT_FAILURE);
            }
        }

        img_count++;
        
        if((img_count%num_file_in_folder)==0)
        {
            cnt++;
            set_directory=true;
        }
        
        if(!quiet)        
            std::cout<< "this is the image read sofar: "<< img_count<< std::endl;

        return((void*) new image_task(filepath, tmp));
    }
    else 
        return NULL;
};



// setup ocl parameter
oclParameter*  set_oclParameter_sobel(cl_device_id dId, std::string kernelPath){
    /* create a CL program using the kernel source */
    //std::string kernelPath ("/users/mehdi/Thesis/fastflow-2.0.2.10-VIP/examples/SobelFilter/SobelFilter_Kernels.cl");
    std::ifstream t(kernelPath.c_str());
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::string source_string = buffer.str() + '\0';
    const char* source =source_string.c_str();                                                                                                                 

    oclParameter* oclParams = new oclParameter(dId);

    //    if(!quiet)    std::cout<< "switching"<<std::endl;
    if(!quiet)    std::cout<< dId<< std::endl;
    cl_int status;
    // creating the context
    oclParams->context = clCreateContext(NULL,1,&dId,NULL,NULL,&status);
    if(!quiet) {
    printStatus("CreatContext: ", status);
    }
 
    size_t sourceSize[] = { strlen(source) };
    if(!quiet) {
      std::cout << sourceSize[0] << " bytes in source" << std::endl;
    }

    oclParams->program = clCreateProgramWithSource(oclParams->context,1,(const char **)&source,sourceSize,&status);
    if(!quiet) {
      printStatus("CreateProgramWithSource: ", status);
    }

    status = clBuildProgram(oclParams->program,1,&dId,NULL,NULL,NULL);
    if(!quiet) {
    printStatus("build: ", status);
    }
    size_t len;
    clGetProgramBuildInfo(oclParams->program, dId, CL_PROGRAM_BUILD_LOG, 0, NULL, &len);
    char* buff = new char[len];
    clGetProgramBuildInfo(oclParams->program, dId, CL_PROGRAM_BUILD_LOG, len, buff, NULL);
    if(!quiet) {
      printf("%s\n", buff);
    }

    //creating command quue
    if(!quiet) {
      printStatus("BuildProgram",status);
    }

    /* The block is to move the declaration of prop closer to its use */
    cl_command_queue_properties prop = 0;
    oclParams->commandQueue = clCreateCommandQueue(oclParams->context, dId, prop, &status);
    if(!quiet) {
      printStatus("CreateCommandQueue",status);
    }

    if(!quiet) {
      printf("run per device\n");
    }
    return oclParams;
  };

  void * execute_kernel_sobel_gpu(void * task, oclParameter* oclParams) 
  {
    if (task==NULL)
    { 
        printf("null\n"); 
        return NULL;
    }
    else
    {
        cl_int status;
        image_task *t =(image_task*)task;
        // get a kernel object handle for a kernel with the given name 
        cl_kernel kernel = clCreateKernel(oclParams->program, "sobel_filter", &status);
        if(!quiet)
            printStatus("CreateKernel",status);

        // Check group size against kernelWorkGroupSize 
        size_t kernelWorkGroupSize;
        status = clGetKernelWorkGroupInfo(kernel, oclParams->d_id,CL_KERNEL_WORK_GROUP_SIZE,sizeof(size_t),&kernelWorkGroupSize,0);
        if(!quiet)
          printStatus("kernel grupsize match checking",status);
    
        size_t blockSizeX =(t->width)/2;                      /**< Work-group size in x-direction */
        size_t blockSizeY=(t->height)/2;                      /**< Work-group size in y-direction */
        cl_event events[2]={0,0};
        size_t globalThreads[2];
        size_t localThreads[2];
        int x_w=0;
        int y_w=0;

        if((blockSizeX * blockSizeY) > kernelWorkGroupSize)
        {
            if(!quiet)
            {
                
                std::cout << "Out of Resources!" << std::endl;
                std::cout << "Group Size specified : "
                          << blockSizeX * blockSizeY << std::endl;
                std::cout << "Max Group Size supported on the kernel : "
                          << kernelWorkGroupSize << std::endl;
                std::cout << "Falling back to " << kernelWorkGroupSize << std::endl;
            }

           // if(blockSizeX > kernelWorkGroupSize)
          //  {

            blockSizeX = static_cast<int> (sqrt(kernelWorkGroupSize));
            blockSizeY = blockSizeX;
            //}
        }
        globalThreads[0] = t->width;
        globalThreads[1] = t->height;
        localThreads[0]  =blockSizeX;
        localThreads[1]  =blockSizeY;

        // create memory object for temp buffer
        cl_mem inputBuffer = clCreateBuffer(oclParams->context,CL_MEM_READ_WRITE,t->width * t->height * sizeof(cl_uchar4),0,&status);
        if(!quiet)  
            printStatus("clCreateBuffer. (tempImageBuffer)",status);

        cl_mem outputBuffer =clCreateBuffer(oclParams->context,CL_MEM_READ_WRITE, t->width * t->height * sizeof(cl_uchar4),NULL,&status);
        if(!quiet) 
            printStatus("createbuf1",status);
        
         // input : input buffer image
        status = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&(inputBuffer));
        if(!quiet) 
            printStatus("setKernel1",status);

        // output : output Buffer
        status = clSetKernelArg(kernel,1, sizeof(cl_mem), &outputBuffer);
        if(!quiet) 
            printStatus("clSetKernelArg(tempImageBuffer).", status);

        status = clSetKernelArg(kernel, 2, sizeof(cl_uint), &(t->width));
        if(!quiet) 
            printStatus("setKernel3",status);

        status = clSetKernelArg(kernel, 3, sizeof(cl_uint), &(t->height));
        if(!quiet) 
            printStatus("setKernel4",status);

        status = clSetKernelArg(kernel, 4, sizeof(int), &(x_w));
        if(!quiet)
            printStatus("setKernel5",status);
        
        status = clSetKernelArg(kernel, 5, sizeof(int), &(y_w));
        if(!quiet)
          printStatus("setKernel6",status);

        // Use clEnqueueWriteBuffer() to write input array A to the device buffer bufferA
        status = clEnqueueWriteBuffer(oclParams->commandQueue,inputBuffer,CL_FALSE,0,t->width * t->height * sizeof(uchar4), t->inputImageData,0,NULL,NULL);
        if(!quiet) printStatus("writebuffe_inputr",status);              

        clFlush(oclParams->commandQueue);       

        // Enqueue a kernel run call.
        status = clEnqueueNDRangeKernel(oclParams->commandQueue,kernel,2,NULL,globalThreads,localThreads,0,NULL,&events[0]);
        if(!quiet) 
            printStatus("enqueueNDRangeKernel",status);
        
        clFlush(oclParams->commandQueue);

        // wait for the kernel call to finish execution 
        status = clWaitForEvents(1, &events[0]);
        if(!quiet) 
            printStatus("events_0",status); 
        
        status = clReleaseEvent(events[0]);
        if(!quiet) 
            printStatus("release_events_0",status); 

        //from here
        for(int i=0; i<t->iter; ++i)
        {

            // output : input Buffer
            status = clSetKernelArg(kernel,0, sizeof(cl_mem), &outputBuffer);
            if(!quiet) 
                printStatus("clSetKernelArg(tempImageBuffer).", status);
            // input : input buffer image
            status = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&(inputBuffer));
            if(!quiet) 
                printStatus("setKernel1",status);

            status = clSetKernelArg(kernel, 2, sizeof(cl_uint), &(t->width));
            if(!quiet) 
                printStatus("setKernel3",status);

            status = clSetKernelArg(kernel, 3, sizeof(cl_uint), &(t->height));
            if(!quiet) 
                printStatus("setKernel4",status);

            status = clSetKernelArg(kernel, 4, sizeof(int), &(x_w));
            if(!quiet)
                printStatus("setKernel5",status);
        
            status = clSetKernelArg(kernel, 5, sizeof(int), &(y_w));
            if(!quiet)
                printStatus("setKernel6",status);


            /* Enqueue a kernel run call.*/
            status = clEnqueueNDRangeKernel(oclParams->commandQueue,kernel,2,NULL,globalThreads,localThreads,0,NULL,&events[0]);
            if(!quiet) printStatus("enqueueNDRangeKernel",status);
            clFlush(oclParams->commandQueue);
            /* wait for the kernel call to finish execution */
            status = clWaitForEvents(1, &events[0]);
            if(!quiet) 
                printStatus("events_0",status); 
            
            status = clReleaseEvent(events[0]);
            if(!quiet) 
                printStatus("release_events_0",status); 

            // third time to get the write one
            // output : input Buffer
            events[0]=0;
            status = clSetKernelArg(kernel,0, sizeof(cl_mem), &inputBuffer);
            if(!quiet) 
                printStatus("clSetKernelArg(tempImageBuffer).", status);

             // input : input buffer image
            status = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&(outputBuffer));
            if(!quiet) 
                printStatus("setKernel1",status);

            status = clSetKernelArg(kernel, 2, sizeof(cl_uint), &(t->width));
            if(!quiet) 
                printStatus("setKernel3",status);

            status = clSetKernelArg(kernel, 3, sizeof(cl_uint), &(t->height));
            if(!quiet) 
                printStatus("setKernel4",status);

            status = clSetKernelArg(kernel, 4, sizeof(int), &(x_w));
            if(!quiet)
                printStatus("setKernel5",status);
        
            status = clSetKernelArg(kernel, 5, sizeof(int), &(y_w));
            if(!quiet)
                printStatus("setKernel6",status);

            // Enqueue a kernel run call.
            status = clEnqueueNDRangeKernel(oclParams->commandQueue,kernel,2,NULL,globalThreads,localThreads,0,NULL,&events[0]);
            if(!quiet) 
                printStatus("enqueueNDRangeKernel",status);
            
            clFlush(oclParams->commandQueue);
            // wait for the kernel call to finish execution 
            status = clWaitForEvents(1, &events[0]);
            if(!quiet) 
                printStatus("events_0",status); 
            
            status = clReleaseEvent(events[0]);
            if(!quiet) 
                printStatus("release_events_0",status); 
        }
        //til here

        // Enqueue readBuffer
        status = clEnqueueReadBuffer(oclParams->commandQueue,outputBuffer,CL_TRUE, 0,t->width * t->height * sizeof(uchar4),t->outputImageData,0,NULL,&events[1]); 
        if(!quiet) 
            printStatus("runbuffer",status); 
        
        clFlush(oclParams->commandQueue);
        // Wait for the read buffer to finish execution 
        status = clWaitForEvents(1, &events[1]);
        if(!quiet) 
            printStatus("event_1",status); 

        status = clReleaseEvent(events[1]);
        if(!quiet) 
            printStatus("release_events_1",status); 

        if(!quiet) 
            printf("GPU done the job!\n"); 

        size_t len;
        clGetDeviceInfo(oclParams->d_id, CL_DEVICE_VENDOR, 0, NULL, &len);
        
        char* vendor = new char[len];
        clGetDeviceInfo(oclParams->d_id, CL_DEVICE_VENDOR, len, vendor, NULL);
        if(!quiet)        
            printf(" the device vendor is %s\n", vendor);
       
        task=t;
       
        clReleaseMemObject(inputBuffer);
        inputBuffer=NULL;
       
        clReleaseMemObject(outputBuffer);
        outputBuffer=NULL;
       
        return task;
    }
};

void * execute_kernel_sobel_cpu(void * task, oclParameter* oclParams) {
    if (task==NULL)
    { 
        printf("null\n"); 
        return NULL;
    }
    else
    {
        cl_int status;
        image_task *t =(image_task*)task;
        // get a kernel object handle for a kernel with the given name 
        cl_kernel kernel = clCreateKernel(oclParams->program, "sobel_filter", &status);
        if(!quiet) 
            printStatus("CreateKernel",status);

        // create memory object for temp buffer
        cl_mem inputBuffer = clCreateBuffer(oclParams->context,CL_MEM_READ_WRITE,t->width * t->height * sizeof(cl_uchar4),0,&status);
        if(!quiet)  
            printStatus("clCreateBuffer. (tempImageBuffer)",status);

        cl_mem outputBuffer =clCreateBuffer(oclParams->context,CL_MEM_READ_WRITE, t->width * t->height * sizeof(cl_uchar4),NULL,&status);
        if(!quiet) 
            printStatus("createbuf1",status);

        // Use clEnqueueWriteBuffer() to write input array A to the device buffer bufferA
        status = clEnqueueWriteBuffer(oclParams->commandQueue,inputBuffer,CL_FALSE,0,t->width * t->height * sizeof(uchar4), t->inputImageData,0,NULL,NULL);
        if(!quiet) 
            printStatus("writebuffe_inputr",status);              
        
        clFlush(oclParams->commandQueue);       
        
        cl_event events[2]={0,0};
        size_t globalThreads[2];
        size_t localThreads[2];
        size_t blockSizeX =(t->width)/2;
        size_t blockSizeY=(t->height)/2;
        size_t kernelWorkGroupSize;
        // Check group size against kernelWorkGroupSize 
        status = clGetKernelWorkGroupInfo(kernel, oclParams->d_id,CL_KERNEL_WORK_GROUP_SIZE,sizeof(size_t),&kernelWorkGroupSize,0);
        if(!quiet)
            printStatus("kernel grupsize match checking",status);

        if((blockSizeX * blockSizeY) > kernelWorkGroupSize)
        {
            if(!quiet)
            {    
                std::cout << "Out of Resources!" << std::endl;
                std::cout << "Group Size specified : "
                          << blockSizeX * blockSizeY << std::endl;
                std::cout << "Max Group Size supported on the kernel : "
                          << kernelWorkGroupSize << std::endl;
                std::cout << "Falling back to " << kernelWorkGroupSize << std::endl;
            }

          //  if(blockSizeX > kernelWorkGroupSize)
            //{
                blockSizeX = static_cast<int> (sqrt(kernelWorkGroupSize));
                blockSizeY = blockSizeX;
            //}
        }

        globalThreads[0] = blockSizeX*4;
        globalThreads[1] = blockSizeY*2;       
        localThreads[0]  =blockSizeX;
        localThreads[1]  =blockSizeY;
        if(globalThreads[0]>(t->width)) globalThreads[0]=(t->width);
        int x_w;
        int y_w;
        int check_range_x = (((t->width))/(globalThreads[0]));
        int check_range_y = ((t->height)/(globalThreads[1]));
        for(int k=0; k<check_range_x; ++k)
        {
            for(int j=0; j<check_range_y; ++j)
            {
                x_w= k*(globalThreads[0]);
                y_w=j*(globalThreads[1]);
                // input : input buffer image
                status = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&(inputBuffer));
                if(!quiet) 
                    printStatus("setKernel1",status);

                // output : output Buffer
                status = clSetKernelArg(kernel,1, sizeof(cl_mem), &outputBuffer);
                if(!quiet) 
                    printStatus("clSetKernelArg(tempImageBuffer).", status);

                status = clSetKernelArg(kernel, 2, sizeof(cl_uint), &(t->width));
                if(!quiet) 
                    printStatus("setKernel3",status);

                status = clSetKernelArg(kernel, 3, sizeof(cl_uint), &(t->height));
                if(!quiet) 
                    printStatus("setKernel4",status);

                status = clSetKernelArg(kernel, 4, sizeof(int), &(x_w));
                if(!quiet)
                    printStatus("setKernel5",status);
                
                status = clSetKernelArg(kernel, 5, sizeof(int), &(y_w));
                if(!quiet)
                  printStatus("setKernel6",status);

                // Enqueue a kernel run call.
                status = clEnqueueNDRangeKernel(oclParams->commandQueue,kernel,2,NULL,globalThreads,localThreads,0,NULL,&events[0]);
                //std::cout<< "THIS IS THE NAME:***************************"<<t->outputImageName<< std::endl; 
                if(!quiet) 
                    printStatus("enqueueNDRangeKernel",status);
                clFlush(oclParams->commandQueue);
                // wait for the kernel call to finish execution 
                
                status = clWaitForEvents(1, &events[0]);
                if(!quiet) 
                    printStatus("events_0",status); 
                
                status = clReleaseEvent(events[0]);
                if(!quiet) 
                    printStatus("release_events_0",status); 
            }
        }

        //from here
        for(int i=0; i<t->iter; ++i)
        {
            for(int k=0; k<check_range_x; ++k)
            {
                for(int j=0; j<check_range_y; ++j)
                {
                    // output : input Buffer
                    x_w= k*(globalThreads[0]);
                    y_w=j*(globalThreads[1]);
                    status = clSetKernelArg(kernel,0, sizeof(cl_mem), &outputBuffer);
                    if(!quiet) 
                        printStatus("clSetKernelArg(tempImageBuffer).", status);
                    // input : input buffer image
                    status = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&(inputBuffer));
                    if(!quiet) 
                        printStatus("setKernel1",status);

                    status = clSetKernelArg(kernel, 2, sizeof(cl_uint), &(t->width));
                    if(!quiet) 
                        printStatus("setKernel3",status);

                    status = clSetKernelArg(kernel, 3, sizeof(cl_uint), &(t->height));
                    if(!quiet) 
                        printStatus("setKernel4",status);

                    status = clSetKernelArg(kernel, 4, sizeof(int), &(x_w));
                    if(!quiet)
                        printStatus("setKernel5",status);
                                
                    status = clSetKernelArg(kernel, 5, sizeof(int), &(y_w));
                    if(!quiet)
                        printStatus("setKernel6",status);

                    // Enqueue a kernel run call.
                    status = clEnqueueNDRangeKernel(oclParams->commandQueue,kernel,2,NULL,globalThreads,localThreads,0,NULL,&events[0]);
                    if(!quiet) 
                        printStatus("enqueueNDRangeKernel",status);
                    
                    clFlush(oclParams->commandQueue);
                    
                    // wait for the kernel call to finish execution 
                    status = clWaitForEvents(1, &events[0]);
                    if(!quiet) 
                        printStatus("events_0",status); 
                
                    status = clReleaseEvent(events[0]);
                    if(!quiet) 
                        printStatus("release_events_0",status); 
                }
            }

            for(int k=0; k<check_range_x; ++k)
            {
                for(int j=0; j<check_range_y; ++j)
                {
                    x_w= k*(globalThreads[0]);
                    y_w=j*(globalThreads[1]);
                    // third time to get the write one
                   // output : input Buffer
                    events[0]=0;

                    status = clSetKernelArg(kernel,0, sizeof(cl_mem), &inputBuffer);
                    if(!quiet) 
                        printStatus("clSetKernelArg(tempImageBuffer).", status);

                    // input : input buffer image
                    status = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&(outputBuffer));
                    if(!quiet) 
                        printStatus("setKernel1",status);

                    status = clSetKernelArg(kernel, 2, sizeof(cl_uint), &(t->width));
                    if(!quiet) 
                        printStatus("setKernel3",status);

                    status = clSetKernelArg(kernel, 3, sizeof(cl_uint), &(t->height));
                    if(!quiet) 
                        printStatus("setKernel4",status);

                    status = clSetKernelArg(kernel, 4, sizeof(int), &(x_w));
                    if(!quiet)
                        printStatus("setKernel5",status);
                                
                    status = clSetKernelArg(kernel, 5, sizeof(int), &(y_w));
                    if(!quiet)
                        printStatus("setKernel6",status);

                    // Enqueue a kernel run call.
                    status = clEnqueueNDRangeKernel(oclParams->commandQueue,kernel,2,NULL,globalThreads,localThreads,0,NULL,&events[0]);
                    if(!quiet) 
                        printStatus("enqueueNDRangeKernel",status);
                    
                    clFlush(oclParams->commandQueue);
                    // wait for the kernel call to finish execution 
                    status = clWaitForEvents(1, &events[0]);
                    if(!quiet) 
                        printStatus("events_0",status); 
                
                    status = clReleaseEvent(events[0]);
                    if(!quiet) 
                        printStatus("release_events_0",status); 
                }
            }
        }
        //til here

        // Enqueue readBuffer
        status = clEnqueueReadBuffer(oclParams->commandQueue,outputBuffer,CL_TRUE, 0,t->width * t->height * sizeof(uchar4),t->outputImageData,0,NULL,&events[1]); 
        if(!quiet) 
            printStatus("runbuffer",status); 
        
        clFlush(oclParams->commandQueue);
        // Wait for the read buffer to finish execution 
        
        status = clWaitForEvents(1, &events[1]);
        if(!quiet) 
            printStatus("event_1",status); 

        status = clReleaseEvent(events[1]);
        if(!quiet) 
            printStatus("release_events_1",status); 

        if(!quiet) 
            printf("GPU done the job!\n"); 

        size_t len;
        clGetDeviceInfo(oclParams->d_id, CL_DEVICE_VENDOR, 0, NULL, &len);

        char* vendor = new char[len];
        clGetDeviceInfo(oclParams->d_id, CL_DEVICE_VENDOR, len, vendor, NULL);
        if(!quiet)        
            printf(" the device vendor is %s\n", vendor);

        task=t;
        
        clReleaseMemObject(inputBuffer);
        inputBuffer=NULL;
        
        clReleaseMemObject(outputBuffer);
        outputBuffer=NULL;
        
        return task;
    }
};

void * execute_kernel_sobel(void * task, oclParameter* oclParams) {
    
    if (task==NULL)
    { 
        printf("null\n"); return NULL;
    }
    else
    {
        cl_device_type d_type;
        clGetDeviceInfo(oclParams->d_id, CL_DEVICE_TYPE, sizeof(cl_device_type), &(d_type), NULL);
        if((d_type)&CL_DEVICE_TYPE_CPU)
        {
            return execute_kernel_sobel_cpu(task, oclParams);
        }
        else
        {
         return execute_kernel_sobel_gpu(task, oclParams);   
        }
    }
};


}

