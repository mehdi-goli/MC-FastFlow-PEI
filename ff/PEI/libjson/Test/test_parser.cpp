#include <iostream>
#include <libjson.h>
#include <fstream>
#include <string>
#include <cerrno>
#include <cxxabi.h>
//#include <string.h>
//#include <stdio.h>
#include "dependency.hpp"
#include <algorithm>
using namespace std;
JSONNode read_formatted(const char *filename){
  std::ifstream in(filename, std::ios::in | std::ios::binary);
  JSONNode n(JSON_NULL);
  if (in){
    std::string contents;
    in.seekg(0, std::ios::end);
    contents.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], contents.size());
    in.close();
    if(!(contents.empty())){
      n = libjson::parse(contents);
    }
  }
  return n;
}

void write_formatted(JSONNode n, const char *filename){
  if(n.type()!=JSON_NULL){
    std::string jc = n.write_formatted();
    std::ofstream out(filename);
    out << jc;
    out.close();
  }
}
class test{

};
int main(){
JSONNode n = read_formatted("input.json");
JSONNode n1 = read_formatted("input1.json");
test *tst = new test();
int status;
//n.set_name(abi::__cxa_demangle(typeid(*tst).name(), 0,0,&status ));
n.push_back(JSONNode("FF::Node_Subclass", abi::__cxa_demangle(typeid(*tst).name(), 0,0,&status )));
if(n1.type()==JSON_NULL){
   std::cout<<"null"<<std::endl; 
}
JSONNode n2 (JSON_NODE);
n2.set_name("Child1");
n2.push_back(JSONNode("id",1));
n.set_name("Parrent");
n.push_back(n2);
JSONNode::iterator it =n.find_nocase("String Node");
if(it->as_string()=="-1"){std::cout<<it->as_int()<<std::endl;}
it->as_int()!=-1 ? (std::cout<< "it is"<<std::endl) : (std::cout<<"Mapper Warning: processor id is -1"<<std::endl);
if (n.at("String Node")==""){
std::cout<<"ha ha ha"<<std::endl;
}
std::cout<< "This is the name: "<<n.name()<<std::endl;
n.at("String Node")="";
bool x =true;
n.push_back(JSONNode("MyBOOLNODE", x));
n["String Node"]=x;
//n.erase(n.find_nocase("String Node"));
write_formatted(n1, "out1.json");
write_formatted(n, "out.json");
JSONNode::const_iterator i =n.find_nocase("ArrayOfNumbers");
std::string strLowerCase= "ARRAYOfNUMbers"; 
std::string myTest= "ff::ff_farm<adaptive_loadbalancer, ff::ff_gatherer>";
std::transform(myTest.begin(), myTest.end(), myTest.begin(), ::tolower);
std::size_t found = myTest.find("adaptive_loadbalancer");
if (found!=std::string::npos)
  std::cout << "first 'needle' found at: " << found << '\n';

std::cout<< "here it is: " << myTest<< std::endl;
JSONNode n_ar = n.at("ArrayOfNumbers");
std::cout<<"here :"<<n_ar[0].as_int()<< std::endl;
// if (0 == strcasecmp("hello", "HELLO"))

if(strcasecmp((i->name()).c_str(), strLowerCase.c_str()) == 0)
//if(!(n2.empty()))
  std::cout<<"haha"<<i->size()<<std::endl;
std::cout<<i->name()<<std::endl;
std::cout<<((i->as_array()).begin()+1)->as_int()<<std::endl;
std::cout<<((i->as_array()).at(1)).as_int()<<std::endl;
std::cout<<((i->as_array())[1]).as_int()<<std::endl;
//std::cout<<i->as_string()<<std::endl;
//JSONNode c(JSON_ARRAY);
//c=i->as_array();
//JSONNode nk= c.at(0);
//JSONNode::const_iterator it = c.begin();
//std::cout <<nk.as_int()<<std::endl;
return 0;
}