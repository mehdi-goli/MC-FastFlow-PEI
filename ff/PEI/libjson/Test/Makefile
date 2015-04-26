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

JSON_ROOT            = ..
JSON_SOURCE	     = #$(JSON_ROOT)/_internal/Source
CC                   = g++
LINK_OPT             = 
VERSION              = 
OPTIMIZE_FLAGS       = -O3
CXXFLAGS             = -DNDEBUG
CFLAGS               =
LDFLAGS              = 
INCS                 = -I$(JSON_ROOT) -I.
LIBS                 = 
DEPENDENCY	     = #$(JSON_SOURCE)/*.cpp
INCLUDES             = $(INCS)
TARGET               = test test_parser
#ff_qs_mpmc_bk ff_qs_mpmc_nbk


.PHONY: all clean cleanall distclean
.SUFFIXES: .c .cpp .o


all: $(TARGET)

test: test.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) $(DEPENDENCY) $(OPTIMIZE_FLAGS) $< -o $@

test_parser: test_parser.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) $(DEPENDENCY) $(OPTIMIZE_FLAGS) $< -o $@

clean: 
	-rm -fr *.o *~ $(TARGET)

cleanall: clean
	-rm -fr $(TARGET) *.d 
