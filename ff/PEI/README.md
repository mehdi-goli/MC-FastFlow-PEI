FastFlow-PEI
============

Experimental PEI Extensions to [FastFlow](http://sourceforge.net/projects/mc-fastflow/). 
#### Additional requirements
PEI uses the ZeroMq, which is provided in the project host platform.
[Install](http://zeromq.org/area:download). Also, the C++ buinding library 
for zmq.hpp can be found [here](https://github.com/zeromq/cppzmq) 
and for zhelpers.hpp can be found [here](https://github.com/imatix/zguide/blob/master/examples/C%2B%2B/zhelpers.hpp) 
### Required Compilation Flags

Compilation Flags The application has to be compiled with the following flags:
-DTRACE_FASTFLOW -DPROFILER -FF_ESAVER -DZMQ_DSRI 
Usage In order to use the PEI functionalities programmers should invoke the 
following method only once for any component in the skeleton tree, before calling
the run function.

#### Usage
To construct an application:

* Add "#include \<ff/PEI/DSRIManagerNode.hpp\>" on top of your application. 
* For each application, invoke "PROFILE" method on the root component to monitor and tune the skeleton tree. This method must be called once per skeleton. This method will activate VIP instrumentation on that subtree. Using this method it is possible to run multiple application (multi-tenant application) efficiently in an interactive mode. 

PROFILE(&comp);
This will activate VIP instrumentation on that subtree for both mapping and
if applicable load-balancing as default. This function can accept 2 more
optional parameter for controlling the level of automisation and application priority.
PROFILE(&comp, boolean);
The boolean value determines whether the sampling mode is sparse or agressive. 
0 is spase and 1 is agressive. The default value is 0.
PROFILE(&comp, boolean, int);
the boolean value determines the sampling mode and the int value is an integer value between (0,99) that determines a value for priority when the application priority is blank. Leaving this value unattended DSRI will auto set the priority for an application.
the auto loadbalancer should
be used.

To execute an application: 

* Adjust the makefile provided in ‘ff/PEI’ folder and compile the DSRI-server by running the makeFile provided in 
‘ff/PEI’
* Execute the DSRI server by running ./server_executer in the ‘ff/PEI folder’. The environmental constraint can be set by modifying the ‘ff/PEI/envConst/env_const.json
* Compile and execute you FastFlow generated application.

After execution, profiling information in JSON format will be output to the 
locations ‘ff/PEI/profiling/executable/applicationname_actuator.json’ and ‘profiling/executable/applicationname_sensor.json’.


Profile information can be visualised with this [JSON Online Viewer](http://jsonviewer.stack.hu/) until some tools become available.


#### Instrumented examples

* Examples/Image_processing/ff-code/simple-convolution
* Examples/Image_processing/ff-code/sobel-filter


#### Disclaimer

This is an early implementation and the final protocol is under development.

#### Authors

Mehdi Goli (<goli.mehdi@gmail.com>).

