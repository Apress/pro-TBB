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
LIBS = -ltbb

# \
!ifndef 0   # \
all clean:  _printmsg # \
# \
_printmsg:  # \
	@echo "for nmake, please specify /f Makefile.nmake"
# \
!else

.PHONY : all clean fig_12_01 fig_12_03 fig_12_04 fig_12_05 fig_12_06 fig_12_07 fig_12_09 fig_12_10 fig_12_11 fig_12_12

all : fig_12_01 fig_12_03 fig_12_04 fig_12_05 fig_12_06 fig_12_07 fig_12_09 fig_12_10 fig_12_11 fig_12_12

fig_12_01 : 
	$(CXX) $(CXXFLAGS) -o fig_12_01 fig_12_01.cpp $(LIBS)

fig_12_03 : 
	$(CXX) $(CXXFLAGS) -o fig_12_03 fig_12_03.cpp $(LIBS)

fig_12_04 : 
	$(CXX) $(CXXFLAGS) -o fig_12_04 fig_12_04.cpp $(LIBS)

fig_12_05 : 
	$(CXX) $(CXXFLAGS) -o fig_12_05 fig_12_05.cpp $(LIBS)

fig_12_06 : 
	$(CXX) $(CXXFLAGS) -o fig_12_06 fig_12_06.cpp $(LIBS)

fig_12_07 : 
	$(CXX) $(CXXFLAGS) -o fig_12_07 fig_12_07.cpp $(LIBS)

fig_12_09 : 
	$(CXX) $(CXXFLAGS) -o fig_12_09 fig_12_09.cpp $(LIBS)

fig_12_10 : 
	$(CXX) $(CXXFLAGS) -o fig_12_10 fig_12_10.cpp $(LIBS)

fig_12_11 : 
	$(CXX) $(CXXFLAGS) -o fig_12_11 fig_12_11.cpp $(LIBS)

fig_12_12 : 
	$(CXX) $(CXXFLAGS) -o fig_12_12 fig_12_12.cpp $(LIBS)

clean : 
	rm -f fig_12_01 fig_12_03 fig_12_04 fig_12_05 fig_12_06 fig_12_07 fig_12_09 fig_12_10 fig_12_11 fig_12_12 fig_12_01.o fig_12_03.o fig_12_04.o fig_12_05.o fig_12_06.o fig_12_07.o fig_12_09.o fig_12_10.o fig_12_11.o fig_12_12.o *.bmp *.txt out*.png


# \
!endif
