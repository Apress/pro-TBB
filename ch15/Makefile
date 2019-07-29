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

.PHONY : all clean fig_15_01 fig_15_02 fig_15_03 fig_15_09 fig_15_11 fig_15_12 note_exceptions

all : fig_15_01 fig_15_02 fig_15_03 fig_15_09 fig_15_11 fig_15_12 note_exceptions

fig_15_01 : 
	$(CXX) $(CXXFLAGS) -o fig_15_01 fig_15_01.cpp $(LIBS)

fig_15_02 : 
	$(CXX) $(CXXFLAGS) -o fig_15_02 fig_15_02.cpp $(LIBS)

fig_15_03 : 
	$(CXX) $(CXXFLAGS) -o fig_15_03 fig_15_03.cpp $(LIBS)

fig_15_09 : 
	$(CXX) $(CXXFLAGS) -o fig_15_09 fig_15_09.cpp $(LIBS)

fig_15_11 : 
	$(CXX) $(CXXFLAGS) -o fig_15_11 fig_15_11.cpp $(LIBS)

fig_15_12 : 
	$(CXX) $(CXXFLAGS) -o fig_15_12 fig_15_12.cpp $(LIBS)

note_exceptions : 
	$(CXX) $(CXXFLAGS) -o note_exceptions note_exceptions.cpp $(LIBS)

clean : 
	rm -f fig_15_01 fig_15_02 fig_15_03 fig_15_09 fig_15_11 fig_15_12 note_exceptions fig_15_01.o fig_15_02.o fig_15_03.o fig_15_09.o fig_15_11.o fig_15_12.o note_exceptions.o *.bmp *.txt out*.png


# \
!endif
