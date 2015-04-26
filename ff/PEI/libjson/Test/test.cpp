#include <iostream>
#include <libjson.h>
#include "dependency.hpp"
int main(){
JSONNode n(JSON_NODE);
n.push_back(JSONNode("String Node", "String Value"));
n.push_back(JSONNode("Integer Node", 42));
n.push_back(JSONNode("Floating Point Node", 3.14));
n.push_back(JSONNode("Boolean Node", true));
JSONNode c(JSON_ARRAY);
c.set_name("ArrayOfNumbers");
c.push_back(JSONNode("", 16));
c.push_back(JSONNode("", 42));
c.push_back(JSONNode("", 128));
n.push_back(c);
std::string jc = n.write_formatted();
std::cout << jc << std::endl;
return 0;
}