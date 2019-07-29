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
LIBS = -ltbb -ltbbmalloc_proxy
CXXFLAGS = $(EXTRA_CXXFLAGS) -std=c++11 -O3
LIBS = -ltbb -ltbbmalloc_proxy

# \
!ifndef 0   # \
all clean:  _printmsg # \
# \
_printmsg:  # \
	@echo "for nmake, please specify /f Makefile.nmake"
# \
!else

.PHONY : all clean fig_10_04 fig_10_06 fig_10_08 fig_10_11-bypass fig_10_11 fig_10_15 wavefront_v0_DAG wavefront_v1_addchild wavefront_v2_feeder wavefront_v3_blockstyle wavefront_v4_recycle wavefront_v5_locality wavefront_v6_global

all : fig_10_04 fig_10_06 fig_10_08 fig_10_11-bypass fig_10_11 fig_10_15 wavefront_v0_DAG wavefront_v1_addchild wavefront_v2_feeder wavefront_v3_blockstyle wavefront_v4_recycle wavefront_v5_locality wavefront_v6_global

fig_10_04 :
	$(CXX) $(CXXFLAGS) -o fig_10_04 fig_10_04.cpp $(LIBS)

fig_10_06 :
	$(CXX) $(CXXFLAGS) -o fig_10_06 fig_10_06.cpp $(LIBS)

fig_10_08 :
	$(CXX) $(CXXFLAGS) -o fig_10_08 fig_10_08.cpp $(LIBS)

fig_10_11-bypass :
	$(CXX) $(CXXFLAGS) -o fig_10_11-bypass fig_10_11-bypass.cpp $(LIBS)

fig_10_11 :
	$(CXX) $(CXXFLAGS) -o fig_10_11 fig_10_11.cpp $(LIBS)

fig_10_15 :
	$(CXX) $(CXXFLAGS) -o fig_10_15 fig_10_15.cpp $(LIBS)

wavefront_v0_DAG :
	$(CXX) $(CXXFLAGS) -o wavefront_v0_DAG wavefront_v0_DAG.cpp $(LIBS)

wavefront_v1_addchild :
	$(CXX) $(CXXFLAGS) -o wavefront_v1_addchild wavefront_v1_addchild.cpp $(LIBS)

wavefront_v2_feeder :
	$(CXX) $(CXXFLAGS) -o wavefront_v2_feeder wavefront_v2_feeder.cpp $(LIBS)

wavefront_v3_blockstyle :
	$(CXX) $(CXXFLAGS) -o wavefront_v3_blockstyle wavefront_v3_blockstyle.cpp $(LIBS)

wavefront_v4_recycle :
	$(CXX) $(CXXFLAGS) -o wavefront_v4_recycle wavefront_v4_recycle.cpp $(LIBS)

wavefront_v5_locality :
	$(CXX) $(CXXFLAGS) -o wavefront_v5_locality wavefront_v5_locality.cpp $(LIBS)

wavefront_v6_global :
	$(CXX) $(CXXFLAGS) -o wavefront_v6_global wavefront_v6_global.cpp $(LIBS)

clean :
	rm -f fig_10_04 fig_10_06 fig_10_08 fig_10_11-bypass fig_10_11 fig_10_15 wavefront_v0_DAG wavefront_v1_addchild wavefront_v2_feeder wavefront_v3_blockstyle wavefront_v4_recycle wavefront_v5_locality wavefront_v6_global fig_10_04.o fig_10_06.o fig_10_08.o fig_10_11-bypass.o fig_10_11.o fig_10_15.o wavefront_v0_DAG.o wavefront_v1_addchild.o wavefront_v2_feeder.o wavefront_v3_blockstyle.o wavefront_v4_recycle.o wavefront_v5_locality.o wavefront_v6_global.o *.bmp *.txt out*.png

# \
!endif
