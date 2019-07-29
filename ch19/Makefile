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

UNAME := $(shell uname)

ifeq ($(UNAME),Linux)
#OpenCL on Linux
FRAMEWORK = -lOpenCL #-Wno-ignored-attributes
endif

ifeq ($(UNAME),Darwin)
# OpenCL on Mac
ifeq ($(CXX),g++)
FRAMEWORK = -framework OpenCL -Wno-unsupported-friend -Wno-deprecated-declarations
else
FRAMEWORK = -framework OpenCL
endif
endif

CXX = icc
CXXFLAGS = $(EXTRA_CXXFLAGS) -O2
LIBS = -ltbb $(FRAMEWORK)

# \
!ifndef 0   # \
all clean:  _printmsg # \
# \
_printmsg:  # \
	@echo "for nmake, please specify /f Makefile.nmake"
# \
!else

.PHONY : all clean fig_19_03 fig_19_05 fig_19_07 fig_19_08 fig_19_09 fig_19_16-subbuffer fig_19_16 fig_19_21 fig_19_22 fig_19_24 fig_19_26 fig_19_27

all : fig_19_03 fig_19_05 fig_19_07 fig_19_08 fig_19_09 fig_19_16-subbuffer fig_19_16 fig_19_21 fig_19_22 fig_19_24 fig_19_26 fig_19_27

fig_19_03 :
	$(CXX) $(CXXFLAGS) -std=c++11 -o fig_19_03 fig_19_03.cpp $(LIBS)

fig_19_05 :
	$(CXX) $(CXXFLAGS) -std=c++11 -o fig_19_05 fig_19_05.cpp $(LIBS)

fig_19_07 :
	$(CXX) $(CXXFLAGS) -std=c++11 -o fig_19_07 fig_19_07.cpp $(LIBS)

fig_19_08 :
	$(CXX) $(CXXFLAGS) -std=c++14 -o fig_19_08 fig_19_08.cpp $(LIBS)

fig_19_09 :
	$(CXX) $(CXXFLAGS) -std=c++11 -o fig_19_09 fig_19_09.cpp $(LIBS)

fig_19_16-subbuffer :
	$(CXX) $(CXXFLAGS) -std=c++11 -o fig_19_16-subbuffer fig_19_16-subbuffer.cpp $(LIBS)

fig_19_16 :
	$(CXX) $(CXXFLAGS) -std=c++11 -o fig_19_16 fig_19_16.cpp $(LIBS)

fig_19_21 :
	$(CXX) $(CXXFLAGS) -std=c++11 -o fig_19_21 fig_19_21.cpp $(LIBS)

fig_19_22 :
	$(CXX) $(CXXFLAGS) -std=c++14 -o fig_19_22 fig_19_22.cpp $(LIBS)

fig_19_24 :
	$(CXX) $(CXXFLAGS) -std=c++14 -o fig_19_24 fig_19_24.cpp $(LIBS)

fig_19_26 :
	$(CXX) $(CXXFLAGS) -std=c++11 -o fig_19_26 fig_19_26.cpp $(LIBS)

fig_19_27 :
	$(CXX) $(CXXFLAGS) -std=c++11 -o fig_19_27 fig_19_27.cpp $(LIBS)

clean :
	rm -f fig_19_03 fig_19_05 fig_19_07 fig_19_08 fig_19_09 fig_19_16-subbuffer fig_19_16 fig_19_21 fig_19_22 fig_19_24 fig_19_26 fig_19_27 fig_19_03.o fig_19_05.o fig_19_07.o fig_19_08.o fig_19_09.o fig_19_16-subbuffer.o fig_19_16.o fig_19_21.o fig_19_22.o fig_19_24.o fig_19_26.o fig_19_27.o *.bmp *.txt out*.png

# \
!endif
