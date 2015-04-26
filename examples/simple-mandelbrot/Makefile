# Makefile
#

# Define MANDEL_NO_DISPLAY if you do not want to display image
# ex: export MANDEL_NO_DISPLAY=1  or
#     setenv MANDEL_NO_DISPLAY 1
#
ifdef MANDEL_NO_DISPLAY
DIS=-DNO_DISPLAY
else
DIS=
endif

CC                   = gcc 
CXX                  = g++ 
LINK_OPT             = 
VERSION              = 
OPTIMIZE_FLAGS       = -g -finline-functions
CXXFLAGS             = -DNO_CMAKE_CONFIG -DTRACE_FASTFLOW #-Wall
CFLAGS               = $(DIS) 
LDFLAGS              = $(DIS) -L/usr/X11R6/lib -L.
INCS                 = -I../../
LIBS                 = -lpthread -lmarX2 -lX11 -lm
PEI		     = -DPROFILER -DFF_ESAVER
INCLUDES             = -I. $(INCS) -I/usr/X11R6/include
TARGET               = mandel_ff
SOURCES		     = mandel_ff.cpp marX2.c
SRC = .
INC = .

.PHONY: all clean cleanall

all:$(TARGET)

libmarX2.a: $(SRC)/marX2.c $(INC)/marX2.h
	gcc -c $(OPTIMIZE_FLAGS) $(CFLAGS) -I/usr/X11R6/include $(INCLUDES) $(PEI) $(SRC)/marX2.c 
	ar -rv libmarX2.a marX2.o
	ranlib libmarX2.a

mandel_ff: mandel_ff.cpp  libmarX2.a
	$(CXX)  $(CXXFLAGS) $(OPTIMIZE_FLAGS)  $(CFLAGS) $(INCLUDES) $(PEI) $(LDFLAGS)  -o mandel_ff mandel_ff.cpp $(LIBS)

clean:
	rm -f libmarX2.a *.o

cleanall: clean
	rm -fr $(TARGET) *~ *.dSYM

depend:
	makedepend $(SOURCES)


# DO NOT DELETE THIS LINE -- make depend depends on it.

marX2.o: marX2.h /usr/include/X11/Xlib.h /usr/include/sys/types.h
marX2.o: /usr/include/sys/appleapiopts.h /usr/include/sys/cdefs.h
marX2.o: /usr/include/machine/types.h /usr/include/i386/types.h
marX2.o: /usr/include/i386/_types.h /usr/include/sys/_types.h
marX2.o: /usr/include/machine/_types.h /usr/include/machine/endian.h
marX2.o: /usr/include/i386/endian.h /usr/include/sys/_endian.h
marX2.o: /usr/include/libkern/_OSByteOrder.h
marX2.o: /usr/include/libkern/i386/_OSByteOrder.h /usr/include/sys/_structs.h
marX2.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
marX2.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
marX2.o: /usr/include/_types.h /usr/include/X11/Xutil.h
marX2.o: /usr/include/X11/keysym.h /usr/include/X11/keysymdef.h
marX2.o: /usr/include/stdio.h /usr/include/stdlib.h /usr/include/available.h
marX2.o: /usr/include/sys/wait.h /usr/include/sys/signal.h
marX2.o: /usr/include/machine/signal.h /usr/include/i386/signal.h
marX2.o: /usr/include/i386/_structs.h /usr/include/sys/resource.h
marX2.o: /usr/include/alloca.h /usr/include/string.h /usr/include/math.h
marX2.o: /usr/include/architecture/i386/math.h
