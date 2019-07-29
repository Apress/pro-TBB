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

.PHONY : all clean fig_6_01 fig_6_06 fig_6_09_STL fig_6_10 fig_6_11 fig_6_12

all : fig_6_01 fig_6_06 fig_6_09_STL fig_6_10 fig_6_11 fig_6_12

fig_6_01 : 
	$(CXX) $(CXXFLAGS) -o fig_6_01 fig_6_01.cpp $(LIBS)

fig_6_06 : 
	$(CXX) $(CXXFLAGS) -o fig_6_06 fig_6_06.cpp $(LIBS)

fig_6_09_STL : 
	$(CXX) $(CXXFLAGS) -o fig_6_09_STL fig_6_09_STL.cpp $(LIBS)

fig_6_10 : 
	$(CXX) $(CXXFLAGS) -o fig_6_10 fig_6_10.cpp $(LIBS)

fig_6_11 : 
	$(CXX) $(CXXFLAGS) -o fig_6_11 fig_6_11.cpp $(LIBS)

fig_6_12 : 
	$(CXX) $(CXXFLAGS) -o fig_6_12 fig_6_12.cpp $(LIBS)

clean : 
	rm -f fig_6_01 fig_6_06 fig_6_09_STL fig_6_10 fig_6_11 fig_6_12 fig_6_01.o fig_6_06.o fig_6_09_STL.o fig_6_10.o fig_6_11.o fig_6_12.o *.bmp *.txt out*.png


# \
!endif
