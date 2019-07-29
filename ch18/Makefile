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

# \
!ifndef 0   # \
all clean:  _printmsg # \
# \
_printmsg:  # \
	@echo "for nmake, please specify /f Makefile.nmake"
# \
!else

UNAME := $(shell uname)

ifeq ($(UNAME),Linux)
#Compile OpenCL on Linux
  CLFRAMEWORK = -lOpenCL
endif

ifeq ($(UNAME),Darwin)
#Compile OpenCL on Mac
  CLFRAMEWORK = -framework OpenCL -Wno-deprecated-declarations
endif

CXX = icc
CXXFLAGS = $(EXTRA_CXXFLAGS) -std=c++11 -O2
LIBS = -ltbb -lpthread

.PHONY : all clean fig_18_03-alternative fig_18_03 fig_18_04 fig_18_11 fig_18_11-svm fig_18_11-CLheader fig_18_11-svmCLheader

all : fig_18_03-alternative fig_18_03 fig_18_04 fig_18_11-CLheader

fig_18_03-alternative :
	$(CXX) $(CXXFLAGS) -o fig_18_03-alternative fig_18_03-alternative.cpp $(LIBS)

fig_18_03 :
	$(CXX) $(CXXFLAGS) -o fig_18_03 fig_18_03.cpp $(LIBS)

fig_18_04 :
	$(CXX) $(CXXFLAGS) -o fig_18_04 fig_18_04.cpp $(LIBS)

fig_18_11-svm :
	$(CXX) $(CXXFLAGS) -o fig_18_11-svm fig_18_11-svm.cpp $(LIBS) $(CLFRAMEWORK)

fig_18_11 :
	$(CXX) $(CXXFLAGS) -o fig_18_11 fig_18_11.cpp $(LIBS) $(CLFRAMEWORK)

fig_18_11-CLheader :
	$(CXX) $(CXXFLAGS) -o fig_18_11-CLheader fig_18_11-CLheader.cpp $(LIBS) $(CLFRAMEWORK)

fig_18_11-svmCLheader :
		$(CXX) $(CXXFLAGS) -o fig_18_11-svmCLheader fig_18_11-svmCLheader.cpp $(LIBS) $(CLFRAMEWORK)

clean :
	rm -f fig_18_03-alternative fig_18_03 fig_18_04 fig_18_11-svm fig_18_11 fig_18_11-svmCLheader fig_18_11-CLheader fig_18_03-alternative.o fig_18_03.o fig_18_04.o fig_18_11-svm.o fig_18_11.o *.bmp *.txt out*.png

# \
!endif
