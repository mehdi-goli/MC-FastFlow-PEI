CXX      	     = g++ 
CXXFLAGS             =

OS                   = $(shell uname)
ifeq ($(strip $(OS)),Darwin)
  OPENCL  	     = -framework OpenCL
else
  OPENCL 	     = -lOpenCL
endif

FF_ROOT  	     = ../../
#Non-standard OpenCL Location
OCL_ROOT 	     = /opt/AMDAPP
# 
OCLFLAGS 	     =
OPTFLAGS 	     = -O3
LDFLAGS  	     = -L$(OCL_ROOT)/lib/x86_64
LIBS 	             =  $(LDFLAGS) $(OPENCL) -lpthread -DFASTFLOW_OPENCL -DTRACE_FASTFLOW
PEI		     = -DPROFILER -DFF_ESAVER
INCLUDES 	     = -I$(FF_ROOT) -I$(OCL_ROOT)/include
TARGETS 	     = oclMap farm_oclMap oclReduce farm_oclReduce pipeFarmMap dotProduct

all: $(TARGETS)

oclMap: oclMap.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(OCLFLAGS) $(OPTFLAGS) -o $@ $< $(LFLAGS) $(LIBS) $(PEI)

oclReduce: oclReduce.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(OCLFLAGS) $(OPTFLAGS) -o $@ $< $(LFLAGS) $(LIBS) $(PEI)

farm_oclMap: farm_oclMap.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(OCLFLAGS) $(OPTFLAGS) -o $@ $< $(LFLAGS) $(LIBS) $(PEI)

farm_oclReduce: farm_oclReduce.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(OCLFLAGS) $(OPTFLAGS) -o $@ $< $(LFLAGS) $(LIBS) $(PEI)

pipeFarmMap: pipeFarmMap.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(OCLFLAGS) $(OPTFLAGS) -o $@ $< $(LFLAGS) $(LIBS) $(PEI)

dotProduct: dotProduct.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(OCLFLAGS) $(OPTFLAGS) -o $@ $< $(LFLAGS) $(LIBS) $(PEI)

clean:
	-rm -f $(TARGETS)

cleanall: clean
	rm -f *~
