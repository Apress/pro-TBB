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
EXTRA_CXXFLAGS = -lhwloc -I $(HWLOC_PREFIX)/include -L $(HWLOC_PREFIX)/lib

CXXFLAGS = -std=c++11 -O2 $(EXTRA_CXXFLAGS)
LIBS = -ltbb

# \
!ifndef 0   # \
all clean:  _printmsg # \
# \
_printmsg:  # \
	@echo "for nmake, please specify /f Makefile.nmake"
# \
!else

.PHONY : all clean fig_20_05 fig_20_06 fig_20_10

all : fig_20_05 fig_20_06 fig_20_10

fig_20_05 :
	$(CXX)  fig_20_05.cpp -o fig_20_05 -std=c++11 -O2 $(LIBS)

fig_20_06 :
	$(CXX)  fig_20_06.cpp -o fig_20_06  $(CXXFLAGS) $(LIBS) -pthread

fig_20_10 :
	$(CXX)  fig_20_10.cpp -o fig_20_10  $(CXXFLAGS) $(LIBS) -pthread

clean :
	rm -f fig_20_05 fig_20_06 fig_20_10 fig_20_05.o fig_20_06.o fig_20_10.o *.bmp *.txt out*.png

# \
!endif
