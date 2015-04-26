# ---------------------------------------------------------------------------
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License version 2 as 
#  published by the Free Software Foundation.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
#  As a special exception, you may use this file as part of a free software
#  library without restriction.  Specifically, if other files instantiate
#  templates or use macros or inline functions from this file, or you compile
#  this file and link it with other files to produce an executable, this
#  file does not by itself cause the resulting executable to be covered by
#  the GNU General Public License.  This exception does not however
#  invalidate any other reasons why the executable file might be covered by
#  the GNU General Public License.
#
# ---------------------------------------------------------------------------

FF_HOME              = ../
CC                   = gcc
LINK_OPT             = 
VERSION              = 
OPTIMIZE_FLAGS       = -g -finline-functions
CXXFLAGS             = -DNO_CMAKE_CONFIG -DTRACE_FASTFLOW #-Wall
CFLAGS               =
LDFLAGS              =
INCS                 = -I. -I$(FF_HOME) -I /usr/local/include
LIBS                 = -L /usr/local/lib/ -lpthread -lzmq -ldl
PEI		     = -DPROFILER -DFF_ESAVER
INCLUDES             = $(INCS)
TARGET               = test1 test2 test3 test4 test5 test6 test7 test8 test9 



.PHONY: all clean cleanall distclean
.SUFFIXES: .c .cpp .o


all: $(TARGET)

test1: test1.cpp
	$(CXX) $(INCS) $(CXXFLAGS) $(OPTIMIZE_FLAGS) $< -o $@ $(LIBS) $(PEI)

test2: test2.cpp
	$(CXX) $(INCS) $(CXXFLAGS) $(OPTIMIZE_FLAGS) $< -o $@ $(LIBS) $(PEI)

test3: test3.cpp
	$(CXX) $(INCS) $(CXXFLAGS) $(OPTIMIZE_FLAGS) $< -o $@ $(LIBS) $(PEI)

test4: test4.cpp
	$(CXX) $(INCS) $(CXXFLAGS) $(OPTIMIZE_FLAGS) $< -o $@ $(LIBS) $(PEI)

test5: test5.cpp
	$(CXX) $(INCS) $(CXXFLAGS) $(OPTIMIZE_FLAGS) $< -o $@ $(LIBS) $(PEI)

test6: test6.cpp
	$(CXX) $(INCS) $(CXXFLAGS) $(OPTIMIZE_FLAGS) $< -o $@ $(LIBS) $(PEI)

test7: test7.cpp
	$(CXX) $(INCS) $(CXXFLAGS) $(OPTIMIZE_FLAGS) $< -o $@ $(LIBS) $(PEI)

test8: test8.cpp
	$(CXX) $(INCS) $(CXXFLAGS) $(OPTIMIZE_FLAGS) $< -o $@ $(LIBS) $(PEI)

test9: test9.cpp
	$(CXX) $(INCS) $(CXXFLAGS) $(OPTIMIZE_FLAGS) $< -o $@ $(LIBS) $(PEI)

clean: 
	-rm -fr *.o *~

cleanall: clean
	-rm -fr $(TARGET) *.d 
