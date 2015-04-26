FF_ROOT              = ../..
CC                   = gcc 
CXX                  = g++ 
LINK_OPT             = 
VERSION              = 
OPTIMIZE_FLAGS       = -O3 -finline-functions
CXXFLAGS             = -DNO_CMAKE_CONFIG -DTRACE_FASTFLOW #-Wall
CFLAGS               =
LDFLAGS              = 
INCS                 = -I../../
LIBS                 = -lpthread
PEI		     = -DPROFILER -DFF_ESAVER
INCLUDES             = -I. -I./include $(INCS)
TARGET               = cholesky blockCholesky


.PHONY: all clean cleanall distclean install uninstall
.SUFFIXES: .c .cpp .o

%.d: %.cpp
	set -e; $(CXX) -MM $(INCLUDES) $(CXXFLAGS) $(PEI) $< \
		| sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
		[ -s $@ ] || rm -f $@
%.d: %.c
	set -e; $(CC) -MM $(INCLUDES) $(CFLAGS) $(PEI) $< \
		| sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
		[ -s $@ ] || rm -f $@
%.o: %.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) $(OPTIMIZE_FLAGS) $(PEI) -c -o $@ $<
%.o: %.c
	$(CC) $(INCLUDES) $(CFLAGS) $(PEI) -c -o $@ $<
%: %.o
	$(CXX) $< -o $@ $(LIBS) $(PEI)


all: $(TARGET)

cholesky:cholesky.o
blockCholesky:blockCholesky.o

clean: 
	-rm -fr *.o *~ cholesky blockCholesky
cleanall: clean
	-rm -fr $(TARGET) *.d 

include $(OBJS:.o=.d)
