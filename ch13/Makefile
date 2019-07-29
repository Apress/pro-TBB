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

LinuxOnly := ""
ifeq ($(OS),Windows_NT)
    MyOS := "Windows"
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        MyOS := "Linux"
        LinuxOnly := "fig_13_01"
    endif
    ifeq ($(UNAME_S),Darwin)
        MyOS := "MacOS"
    endif
endif

.PHONY : all clean $(LinuxOnly) fig_13_03 fig_13_05

all : $(LinuxOnly) fig_13_03 fig_13_05

ifeq ($(UNAME_S),Linux)
fig_13_01 : 
	$(CXX) $(CXXFLAGS) -o fig_13_01 fig_13_01.cpp $(LIBS)
endif

fig_13_03 : 
	$(CXX) $(CXXFLAGS) -o fig_13_03 fig_13_03.cpp $(LIBS)

fig_13_05 : 
	$(CXX) $(CXXFLAGS) -o fig_13_05 fig_13_05.cpp $(LIBS)

clean : 
	rm -f fig_13_03 fig_13_05 fig_13_03.o fig_13_05.o *.bmp *.txt out*.png
ifeq ($(UNAME_S),Linux)
	rm -f $(LinuxOnly) fig_13_01.o
endif


# \
!endif
