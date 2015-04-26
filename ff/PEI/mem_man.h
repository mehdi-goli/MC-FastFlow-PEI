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


/*
 * C program to print the system statistics like system uptime, 
 * total RAM space, free RAM space, process count, page size
 */
#ifndef _MEMMAN_HPP
#define _MEMMAN_HPP

#include <sys/sysinfo.h>    // sysinfo
#include <cstdio>
#include <iostream>
#include <unistd.h>     // sysconf

#include <cstdlib>
#include <pthread.h>

//#define MAX_RAM_PRESSURE_WAIT 0.3
//#define MIN_RAM_PRESSURE_SIGNAL 0.3

//the sysinfo contains:
/*
    long hours;
    long min; 
    long sec;
    long total_ram;
    long free_ram;
    long used_ram;
    int process_count;
    long page_size;
};*/
double MAX_RAM_PRESSURE_WAIT=0.3;
double MIN_RAM_PRESSURE_SIGNAL=0.3;
void set_memory_throtling_threshold(double th){
  MAX_RAM_PRESSURE_WAIT=th;
  MIN_RAM_PRESSURE_SIGNAL=th;
}


double get_free_ram(){
  
  struct sysinfo info;
  if (sysinfo(&info) != 0)
     printf("sysinfo: error reading system statistics");
  return ((double)(1.0*info.freeram))/info.totalram;
}

pthread_mutex_t count_mutex     = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condition_var   = PTHREAD_COND_INITIALIZER;

pthread_mutex_t count_mutex_suspend     = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condition_var_suspend   = PTHREAD_COND_INITIALIZER;

void suspend_node()
{
  pthread_mutex_lock( &count_mutex_suspend );
  pthread_cond_wait( &condition_var_suspend, &count_mutex_suspend );
  pthread_mutex_unlock( &count_mutex_suspend );  
  //std::cout<<"_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_mutextedUnlucked_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_"<<std::endl;
}
void resume_node()
{
  pthread_mutex_lock( &count_mutex_suspend );
  pthread_cond_signal( &condition_var_suspend ); 
  pthread_mutex_unlock( &count_mutex_suspend );   
 // std::cout<<"_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+I am resume+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_"<<std::endl;
}

void resume_broadcast_node()
{
  pthread_mutex_lock( &count_mutex_suspend );
  pthread_cond_broadcast( &condition_var_suspend ); 
  pthread_mutex_unlock( &count_mutex_suspend );   
 // std::cout<<"_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+I am resume+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_"<<std::endl;
}
void waitSignal(){

  pthread_mutex_lock( &count_mutex );
  
  if (get_free_ram() < MAX_RAM_PRESSURE_WAIT){
    std::cerr << "Free RAM below limit, forcing wait " << get_free_ram() << std::endl;
    pthread_cond_wait( &condition_var, &count_mutex );
  }
  
  pthread_mutex_unlock( &count_mutex );	 

}
	 
void wakeSignal(){
  
  pthread_mutex_lock( &count_mutex );

  if (get_free_ram() > MIN_RAM_PRESSURE_SIGNAL){
    std::cerr << "Free Mem above limit, continuing " << get_free_ram() << std::endl;
    pthread_cond_signal( &condition_var );
  }
  pthread_mutex_unlock( &count_mutex );
 
}
#endif
