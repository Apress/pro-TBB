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

.PHONY : all clean fig_2_03 fig_2_04 fig_2_05 fig_2_06 fig_2_07 fig_2_08 fig_2_10 fig_2_11 fig_2_12 fig_2_14 fig_2_15 fig_2_16 fig_2_17 fig_2_18 fig_2_19 fig_2_20 fig_2_22 fig_2_23 fig_2_24 fig_2_27 fig_2_28 fig_2_30 sidebar_associativity sidebar_pfor_functor

all : fig_2_03 fig_2_04 fig_2_05 fig_2_06 fig_2_07 fig_2_08 fig_2_10 fig_2_11 fig_2_12 fig_2_14 fig_2_15 fig_2_16 fig_2_17 fig_2_18 fig_2_19 fig_2_20 fig_2_22 fig_2_23 fig_2_24 fig_2_27 fig_2_28 fig_2_30 sidebar_associativity sidebar_pfor_functor

fig_2_03 : 
	$(CXX) $(CXXFLAGS) -o fig_2_03 fig_2_03.cpp $(LIBS)

fig_2_04 : 
	$(CXX) $(CXXFLAGS) -o fig_2_04 fig_2_04.cpp $(LIBS)

fig_2_05 : 
	$(CXX) $(CXXFLAGS) -o fig_2_05 fig_2_05.cpp $(LIBS)

fig_2_06 : 
	$(CXX) $(CXXFLAGS) -o fig_2_06 fig_2_06.cpp $(LIBS)

fig_2_07 : 
	$(CXX) $(CXXFLAGS) -o fig_2_07 fig_2_07.cpp $(LIBS)

fig_2_08 : 
	$(CXX) $(CXXFLAGS) -o fig_2_08 fig_2_08.cpp $(LIBS)

fig_2_10 : 
	$(CXX) $(CXXFLAGS) -o fig_2_10 fig_2_10.cpp $(LIBS)

fig_2_11 : 
	$(CXX) $(CXXFLAGS) -o fig_2_11 fig_2_11.cpp $(LIBS)

fig_2_12 : 
	$(CXX) $(CXXFLAGS) -o fig_2_12 fig_2_12.cpp $(LIBS)

fig_2_14 : 
	$(CXX) $(CXXFLAGS) -o fig_2_14 fig_2_14.cpp $(LIBS)

fig_2_15 : 
	$(CXX) $(CXXFLAGS) -o fig_2_15 fig_2_15.cpp $(LIBS)

fig_2_16 : 
	$(CXX) $(CXXFLAGS) -o fig_2_16 fig_2_16.cpp $(LIBS)

fig_2_17 : 
	$(CXX) $(CXXFLAGS) -o fig_2_17 fig_2_17.cpp $(LIBS)

fig_2_18 : 
	$(CXX) $(CXXFLAGS) -o fig_2_18 fig_2_18.cpp $(LIBS)

fig_2_19 : 
	$(CXX) $(CXXFLAGS) -o fig_2_19 fig_2_19.cpp $(LIBS)

fig_2_20 : 
	$(CXX) $(CXXFLAGS) -o fig_2_20 fig_2_20.cpp $(LIBS)

fig_2_22 : 
	$(CXX) $(CXXFLAGS) -o fig_2_22 fig_2_22.cpp $(LIBS)

fig_2_23 : 
	$(CXX) $(CXXFLAGS) -o fig_2_23 fig_2_23.cpp $(LIBS)

fig_2_24 : 
	$(CXX) $(CXXFLAGS) -o fig_2_24 fig_2_24.cpp $(LIBS)

fig_2_27 : 
	$(CXX) $(CXXFLAGS) -o fig_2_27 fig_2_27.cpp $(LIBS)

fig_2_28 : 
	$(CXX) $(CXXFLAGS) -o fig_2_28 fig_2_28.cpp $(LIBS)

fig_2_30 : 
	$(CXX) $(CXXFLAGS) -o fig_2_30 fig_2_30.cpp $(LIBS)

sidebar_associativity : 
	$(CXX) $(CXXFLAGS) -o sidebar_associativity sidebar_associativity.cpp $(LIBS)

sidebar_pfor_functor : 
	$(CXX) $(CXXFLAGS) -o sidebar_pfor_functor sidebar_pfor_functor.cpp $(LIBS)

clean : 
	rm -f fig_2_03 fig_2_04 fig_2_05 fig_2_06 fig_2_07 fig_2_08 fig_2_10 fig_2_11 fig_2_12 fig_2_14 fig_2_15 fig_2_16 fig_2_17 fig_2_18 fig_2_19 fig_2_20 fig_2_22 fig_2_23 fig_2_24 fig_2_27 fig_2_28 fig_2_30 sidebar_associativity sidebar_pfor_functor fig_2_03.o fig_2_04.o fig_2_05.o fig_2_06.o fig_2_07.o fig_2_08.o fig_2_10.o fig_2_11.o fig_2_12.o fig_2_14.o fig_2_15.o fig_2_16.o fig_2_17.o fig_2_18.o fig_2_19.o fig_2_20.o fig_2_22.o fig_2_23.o fig_2_24.o fig_2_27.o fig_2_28.o fig_2_30.o sidebar_associativity.o sidebar_pfor_functor.o *.bmp *.txt out*.png


# \
!endif
