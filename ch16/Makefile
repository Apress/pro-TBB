# Copyright (C) 2019 Intel Corporation
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom
# the Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
# OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
# ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
# OR OTHER DEALINGS IN THE SOFTWARE.
#
# SPDX-License-Identifier: MIT

CXX = icc
CXXFLAGS = $(EXTRA_CXXFLAGS) -std=c++11 -O2
LIBS = -ltbb -lpthread

# \
!ifndef 0   # \
all clean:  _printmsg # \
# \
_printmsg:  # \
	@echo "for nmake, please specify /f Makefile.nmake"
# \
!else

ifeq ($(CXX),g++)
GCCokay = -Wno-uninitialized
else
GCCokay =
endif

.PHONY : all clean fig_16_04 fig_16_06 fig_16_09 fig_16_11 fig_16_14 fig_16_16 fig_16_17 fig_16_19 fig_16_20 fig_16_21 fig_16_22 fig_16_23 fig_16_24 fig_16_25 fig_16_26

all : fig_16_04 fig_16_06 fig_16_09 fig_16_11 fig_16_14 fig_16_16 fig_16_17 fig_16_19 fig_16_20 fig_16_21 fig_16_22 fig_16_23 fig_16_24 fig_16_25 fig_16_26

fig_16_04 : 
	$(CXX) $(CXXFLAGS) -o fig_16_04 fig_16_04.cpp $(LIBS)

fig_16_06 : 
	$(CXX) $(CXXFLAGS) -o fig_16_06 fig_16_06.cpp $(LIBS)

fig_16_09 : 
	$(CXX) $(CXXFLAGS) -o fig_16_09 fig_16_09.cpp $(LIBS)

fig_16_11 : 
	$(CXX) $(CXXFLAGS) -o fig_16_11 fig_16_11.cpp $(LIBS)

fig_16_14 : 
	$(CXX) $(CXXFLAGS) -o fig_16_14 fig_16_14.cpp $(LIBS)

fig_16_16 : 
	$(CXX) $(CXXFLAGS) -o fig_16_16 fig_16_16.cpp $(LIBS)

fig_16_17 : 
	$(CXX) $(CXXFLAGS) -o fig_16_17 fig_16_17.cpp $(LIBS)

fig_16_19 : 
	$(CXX) $(CXXFLAGS) $(GCCokay) -o fig_16_19 fig_16_19.cpp $(LIBS)

fig_16_20 : 
	$(CXX) $(CXXFLAGS) $(GCCokay) -o fig_16_20 fig_16_20.cpp $(LIBS)

fig_16_21 : 
	$(CXX) $(CXXFLAGS) $(GCCokay) -o fig_16_21 fig_16_21.cpp $(LIBS)

fig_16_22 : 
	$(CXX) $(CXXFLAGS) $(GCCokay) -o fig_16_22 fig_16_22.cpp $(LIBS)

fig_16_23 : 
	$(CXX) $(CXXFLAGS) -o fig_16_23 fig_16_23.cpp $(LIBS)

fig_16_24 : 
	$(CXX) $(CXXFLAGS) -o fig_16_24 fig_16_24.cpp $(LIBS)

fig_16_25 : 
	$(CXX) $(CXXFLAGS) -o fig_16_25 fig_16_25.cpp $(LIBS)

fig_16_26 : 
	$(CXX) $(CXXFLAGS) -o fig_16_26 fig_16_26.cpp $(LIBS)

clean : 
	rm -f fig_16_04 fig_16_06 fig_16_09 fig_16_11 fig_16_14 fig_16_16 fig_16_17 fig_16_19 fig_16_20 fig_16_21 fig_16_22 fig_16_23 fig_16_24 fig_16_25 fig_16_26 fig_16_04.o fig_16_06.o fig_16_09.o fig_16_11.o fig_16_14.o fig_16_16.o fig_16_17.o fig_16_19.o fig_16_20.o fig_16_21.o fig_16_22.o fig_16_23.o fig_16_24.o fig_16_25.o fig_16_26.o *.bmp *.txt out*.png


# \
!endif
