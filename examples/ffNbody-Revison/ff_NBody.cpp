#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <ff/farm.hpp>
#include <ff/node.hpp>
#include <sys/time.h>
#include <math.h>
#include <ff/pipeline.hpp>
#include <stdio.h>
#include <ff/PEI/DSRIManagerNode.hpp>
double get_current_time();
using namespace ff;
#define NBODIES 1024
#define EPS2 0.01
#define dt 0.001
#define NWORKERS 8
#define WSIZE 16
#define HSIZE 16
#define OFFSET NWORKERS
struct particle
{
  double w;
  double x,y,z;
  double vx,vy, vz;
};

struct task_e {
  task_e(){}
  double e;
};

struct task_res {
  task_res(){}
  unsigned int sid;
  unsigned int s1id;
  particle ps[HSIZE];
  particle ps1[HSIZE];
};
struct task_t {
  task_t(int id):id(id) {}
  unsigned int id;
  particle bodies[NBODIES];
};

struct task_p {
  task_p() {}
  double x;
  double y;
  double z;
};

struct task_m {
  task_m(int id):id(id){}
  unsigned int id;
};

struct task_ad {
  task_ad(int hid,int wid):hid(hid),wid(wid){};
  unsigned int hid;
  unsigned int wid;
};

struct pos_t{
  double x;
  double y;
  double z;
};


struct particle bodies[NBODIES];
//static double e[NWORKERS];
//static pos_t p[NWORKERS];
//static particle ad[NBODIES][NWORKERS];
static double a_time[NWORKERS];
//read from input
void get_input(){
  double x;
  int k=0;
  for(int g=0; g<1; g++){                                                                                                    
    const char InputFileName[] ="./input.txt";
    std::ifstream InFile(InputFileName);
    if (!InFile) {
    std::cerr << "File open failure\n";
    std::exit(EXIT_FAILURE);
  }
    while (InFile >> x){
      int l = k%NBODIES;
      if (k/NBODIES == 0) bodies[l].w=x;
      else if(k/NBODIES == 1) bodies[l].x=x;
      else if(k/NBODIES == 2) bodies[l].y=x;
      else if(k/NBODIES == 3) bodies[l].z=x;
      else if(k/NBODIES == 4) bodies[l].vx=x;
      else if(k/NBODIES == 5) bodies[l].vy=x;
      else if(k/NBODIES == 6) bodies[l].vz=x;
      k++;
    }
  }
}
task_res * advance (int m, int wd, int wid){
  double t_s = get_current_time();
  struct particle p[HSIZE];
  struct particle p2[HSIZE];
  for(int i=0; i<HSIZE; i++){
    p[i].x =0;
    p[i].y=0;
    p[i].z=0;
    p[i].vx =0;
    p[i].vy=0;
    p[i].vz=0;
    p[i].w =0;
    p2[i].x =0;
    p2[i].y=0;
    p2[i].z=0;
    p2[i].vx =0;
    p2[i].vy=0;
    p2[i].vz=0;
    p2[i].w =0;
  }
  int n = m+HSIZE;
  int l= wd+WSIZE;
  if (l>NBODIES) l-=1;
  int index=wd;
  struct particle p1;
  struct particle ap;
  for(int i= m; i<n; i++ ){
    p1.x =bodies[i].x;
    p1.y=bodies[i].y;
    p1.z=bodies[i].z;
    p1.vx =bodies[i].vx;
    p1.vy=bodies[i].vy;
    p1.vz=bodies[i].vz;
    p1.w =bodies[i].w;
    if(index<=i) index=i+1;
    for(int k= index ; k<l; k++){
      ap.x =bodies[k].x;
      ap.y=bodies[k].y;
      ap.z=bodies[k].z;
      ap.vx =bodies[k].vx;
      ap.vy=bodies[k].vy;
      ap.vz=bodies[k].vz;
      ap.w =bodies[k].w;
      double dx = p1.x - ap.x;
      double dy = p1.y - ap.y;
      double dz = p1.z - ap.z;
      double distance = sqrt(dx * dx + dy * dy + dz * dz + EPS2);
      double mag = dt / (distance * distance * distance);
      p[i-m].vx -= dx * ap.w * mag;
      p[i-m].vy -= dy * ap.w * mag;
      p[i-m].vz -= dz * ap.w * mag;
      p2[k-wd].vx += dx * p1.w * mag;
      p2[k-wd].vy += dy * p1.w * mag;
      p2[k-wd].vz += dz * p1.w * mag;
    }
  }
  task_res *t_res = new task_res;
  t_res->sid = m;
  t_res->s1id = wd;
  for(int i=0; i<HSIZE; i++){
    t_res->ps1[i].vx =p2[i].vx;
    t_res->ps1[i].vy=p2[i].vy;
    t_res->ps1[i].vz=p2[i].vz;
    t_res->ps[i].vx=p[i].vx;
      t_res->ps[i].vy=p[i].vy;
      t_res->ps[i].vz=p[i].vz;
  }
  double t_e = get_current_time();
  double t_ad = t_e-t_s;
  a_time[wid]+=t_ad;
  return t_res;
}

double energy(int m,int wd, int wid){
  double t_s = get_current_time();
  double e=0.0;
  int n = m+HSIZE;
  int l= wd+WSIZE;
  if (l>NBODIES) l-=1;
    int index =wd;
  for(int i= m; i<n; i++ ){
    struct particle p1;
    p1.x =bodies[i].x;
    p1.y=bodies[i].y;
    p1.z=bodies[i].z;
    p1.vx =bodies[i].vx;
    p1.vy=bodies[i].vy;
    p1.vz=bodies[i].vz;
    p1.w =bodies[i].w;
    if(wd-m==1){ 
      e += 0.5 * p1.w * (p1.vx * p1.vx + p1.vy * p1.vy + p1.vz * p1.vz);
    }
    if(index<=i) index=i+1;
    for(int k= index; k<l; k++){
      struct particle ap;
      ap.x =bodies[k].x;
      ap.y=bodies[k].y;
      ap.z=bodies[k].z;
      ap.vx =bodies[k].vx;
      ap.vy=bodies[k].vy;
      ap.vz=bodies[k].vz;
      ap.w =bodies[k].w;
      double dx = p1.x - ap.x;
      double dy = p1.y - ap.y;
      double dz = p1.z - ap.z;
      double distance = sqrt(dx * dx + dy * dy + dz * dz + EPS2);
      e -= (p1.w*ap.w)/distance;
    }
  }
  return e;
}

task_p* offset_momentum(int m,int wid){
  double x=0;
  double y=0;
  double z=0;
  for(int i=m; i<m+OFFSET;i++ ){
    double vx= bodies[i].vx;
    double vy=bodies[i].vy;
    double vz=bodies[i].vz;
    double w = bodies[i].w;
    x+=vx*w;
    y+=vy*w;
    z+=vz*w;
  }
  task_p * t = new task_p;
  t->x=x;
  t->y=y;
  t->z=z;
  return t;
}

class worker_momentom : public ff_node{
public:
  worker_momentom(int wid):wid(wid){}
    int svc_init(){
      return 0;
    }
    void * svc(void * task) {
      task_m * t = (task_m *)task;
      int m= t->id;
      return (void *)offset_momentum(m, wid);
  }
private:
  int wid;
};

class worker_energy : public ff_node {
public:
  worker_energy(int wid):wid(wid){}
  int svc_init() {
    return 0;
  }
  void * svc(void * task) {
    task_ad * t = (task_ad *)task;
    int m= t->hid;
    int wd=t->wid;
    task_e * tsk_e = new task_e;
    double e =0;
    e= energy(m,wd,wid);
	  tsk_e->e=e;
    delete t;
    return tsk_e;
  }
private:
  int wid;
};


class worker_advance : public ff_node {
public:
  worker_advance(int wid):wid(wid){}
  int svc_init(){
    return 0;
  }
  void * svc(void * task){    
  	task_ad * t = (task_ad *)task;
  	int m = t->hid;
  	int wd= t->wid;
  	task_res * t3= advance(m,wd ,wid);;
  	delete t;
    return t3;
  }
private:
  int wid;
};

class Collector_momentom: public ff_node {
public:
  Collector_momentom() {}

  int svc_init() {
    double t_s = get_current_time();
    tm=0;
    x=0;
    y=0;
    z=0;
    double t_e = get_current_time();
    double t_ad = t_e-t_s;
    tm+=t_ad;
    return 0;
  }

  void * svc(void * task) {
    double t_s = get_current_time();
    task_p* tsk_p= (task_p*)task;
    x+=tsk_p->x;
    y+=tsk_p->y;
    z+=tsk_p->z;
    double t_e = get_current_time();
    double t_ad = t_e-t_s;
    tm+=t_ad;
    delete tsk_p;
    return GO_ON;
  }
  void svc_end(){
    double vx =bodies[0].vx;
	  double vy=bodies[0].vy;
	  double vz=bodies[0].vz;
	  double w= bodies[0].w;
	  vx=-x/w;
	  vy=-y/w;
	  vz=-z/w;
	  bodies[0].vx = vx;
    bodies[0].vy = vy;
    bodies[0].vz = vz;
    printf("collector_time_momentum:= %.15f\n", tm);
  }

private:
  double x;
  double y;
  double z;
  double tm;
};




class Collector_energy: public ff_node {
public:
  Collector_energy(){}

  int svc_init(){
    double t_s = get_current_time();
    tm=0;
    gathered_energy=0;
    double t_e = get_current_time();
    double t_ad = t_e-t_s;
    tm+=t_ad;
    return 0;
  }

  void * svc(void * task) {
    double t_s = get_current_time();
    task_e * t = (task_e*)task;
    gathered_energy+= t->e;
    double t_e = get_current_time();
    double t_ad = t_e-t_s;
    tm+=t_ad;
    delete t;
    return GO_ON;
  }
  void svc_end()
  {
   printf("collected energy :=%.15f\n", gathered_energy);
   printf("collector_time:= %.15f\n", tm);
  }

private:
  double gathered_energy;
  double tm;
};

class Collector_advance: public ff_node{
public:
  Collector_advance(){}
  int svc_init(){
    tm=0;
    double t_s = get_current_time();
    for(int i=0; i<NBODIES; i++){
      gathered_bodies[i].vx =0;
      gathered_bodies[i].vy=0;
      gathered_bodies[i].vz=0;
    }
    double t_e = get_current_time();
    double t_ad = t_e-t_s;
    tm+=t_ad;
    return 0;
  }

  void * svc(void * task) {
    double t_s = get_current_time();
    task_res * t = (task_res*)task;
    int m=t->sid;
    int wd=t->s1id;
    for (int i=0; i<HSIZE-1; i++){
      gathered_bodies[i+m].vx+= t->ps[i].vx;
      gathered_bodies[i+m].vy+= t->ps[i].vy;
      gathered_bodies[i+m].vz+= t->ps[i].vz;
      gathered_bodies[i+wd].vx+= t->ps1[i].vx;
      gathered_bodies[i+wd].vy+= t->ps1[i].vy;
      gathered_bodies[i+wd].vz+= t->ps1[i].vz;
    }
    double t_e = get_current_time();
    double t_ad = t_e-t_s;
    tm+=t_ad;
    delete t;
    return GO_ON;
  }
  void svc_end(){
    double t_s = get_current_time();
    for(int i=0; i<NBODIES; i++){
      struct particle *b = &(bodies[i]);
      b->vx+=gathered_bodies[i].vx;
      b->vy+=gathered_bodies[i].vy;
      b->vz+=gathered_bodies[i].vz; 
      b->x += dt * b->vx;
      b->y += dt * b->vy;
      b->z += dt * b->vz;
    }
    double t_e = get_current_time();
    double t_ad = t_e-t_s;
    tm+=t_ad;
  }

private:
  particle gathered_bodies[NBODIES];
  double tm;
};

// the load-balancer filter    
class Emitter: public ff_node {
public:
  Emitter(){};
  
  void * svc(void *) {
    for(int i=0; i<NBODIES; i+=NWORKERS)
    {
      task_m * t = new task_m(i);
      ff_send_out(t);
    }
    return ((void *)FF_EOS);
  }
};
    
class Emitter_ad: public ff_node {
public:
  Emitter_ad(){};

  void * svc(void *) {
    for (int i=0;i<NBODIES; i+=HSIZE)
	for(int j=i+1;j<NBODIES; j+=WSIZE){
	  task_ad * task = new task_ad(i,j);
	  ff_send_out(task);
	}
    return((void *)FF_EOS);
  }
};
/*void fn_momentom(){
  double px = 0.0, py = 0.0, pz = 0.0;
  for (int i=0; i<NWORKERS; i++){
    px+=p[i].x;
    py+=p[i].y;
    pz+=p[i].z;
  }
  bodies[0].vx = - px / bodies[0].w;
  bodies[0].vy = - py / bodies[0].w;
  bodies[0].vz = - pz / bodies[0].w;
}

double fn_energy(){
  double energy = 0.0;
  for (int i=0; i<NWORKERS; i++){
    energy+=e[i];
	}
  return energy;
}*/

struct res{
  double e1;
  double e2;
  double t;
};
#define STAT 100
int main() {
double cpu_start_time, cpu_end_time;
double total_cpu_start_time, total_cpu_end_time;

    /* init */
 double e1=0.0, e2=0.0,t=0.0;
  res result[STAT];
  ff_farm<> farm_m;
  std::vector<ff_node *> w_m;
  std::cout << "Momentum" << std::endl;
  for(int i=0;i<NWORKERS;++i) w_m.push_back(new worker_momentom(i));
  farm_m.add_workers(w_m);
  Emitter e_m;
  farm_m.add_emitter(&e_m);
  Collector_momentom c_m;
  farm_m.add_collector(&c_m);
 
  ff_farm<> farm_e;
  std::vector<ff_node *> w_e;
  std::cout << "Energy" << std::endl;
  for(int i=0;i<NWORKERS;++i) w_e.push_back(new worker_energy(i));
  farm_e.add_workers(w_e); 
  Emitter_ad e_e;
  farm_e.add_emitter(&e_e);
  Collector_energy c_e;
  farm_e.add_collector(&c_e);
 
 ff_farm<adaptive_loadbalancer> farm_a;
  std::vector<ff_node *> w_a;
  std::cout << "Advance" << std::endl;
  for(int i=0;i<NWORKERS;++i) w_a.push_back(new worker_advance(i));
  farm_a.add_workers(w_a);
  Emitter_ad e_a;
  farm_a.add_emitter(&e_a);
  Collector_advance c_a;
  farm_a.add_collector(&c_a);
  PROFILE(&farm_a);
total_cpu_start_time =get_current_time();
 for (int k=0; k<STAT; k++) {
  get_input();
  cpu_start_time = get_current_time();
  double t_m_s = get_current_time();
  
  farm_m.run_then_freeze();
  farm_m.wait_freezing();
  //farm_m.run_and_wait_end();
  double t_m_e = get_current_time();
  double t_m= t_m_e-t_m_s;
  printf("momentumtime:= %.15f\n", t_m);
  
  farm_e.run_then_freeze();
  farm_e.wait_freezing();
  for(int i=0;i<NWORKERS;++i)a_time[i]=0;
  double t_a_s = get_current_time();
  
  for (int i=1; i<=20; i++){                     
    farm_a.run_then_freeze();
    farm_a.wait_freezing();
  }                                                                              
  double t_a_e = get_current_time();
  double t_a= t_a_e-t_a_s;
  printf("advancetime:= %.15f\n", t_a);
  double t_e_s = get_current_time();

  farm_e.run_then_freeze();
  farm_e.wait_freezing();
  double t_e_e = get_current_time();
  double t_e= t_e_e-t_e_s;
  printf("energytime:= %.15f\n", t_e);
  cpu_end_time = get_current_time();
  t= cpu_end_time - cpu_start_time;
  printf("time:= %.15f\n", t);
  result[k].e1=e1;
  result[k].e2=e2;
  result[k].t=t;
}
total_cpu_end_time=get_current_time();
for(int i=0;i<NWORKERS;++i) printf("advancetime for %d := %.15f\n", i , a_time[i]);
FILE * pFile;
int j;
pFile = fopen ( "./out_ff_result.txt","w");
for (j=0 ; j<STAT ; j++)

fprintf (pFile, "e1: %.15f\te2: %.15f\ttime: %.15f\n ",  result[j].e1,  result[j].e2,  result[j].t);
fclose (pFile);
std::cout<< "Total Time: " << total_cpu_end_time - total_cpu_start_time<< std::endl;
  std::cerr << "DONE, time= " << farm_a.ffTime() << " (ms)\n";
    return 0;
}
double get_current_time(){
  static int start = 0, startu = 0;
  struct timeval tval;
  double result;

  if (gettimeofday(&tval, NULL) == -1)
    result = -1.0;
  else if(!start) {
    start = tval.tv_sec;
    startu = tval.tv_usec;
    result = 0.0;
  }
  else
    result = (double) (tval.tv_sec - start) + 1.0e-6*(tval.tv_usec - startu);

  return result;
}