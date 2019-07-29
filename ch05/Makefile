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

.PHONY : all clean fig_5_03 fig_5_04 fig_5_08 fig_5_14 fig_5_19 fig_5_20 fig_5_20_cacheAligned fig_5_22 fig_5_23 fig_5_24 fig_5_25 fig_5_26 fig_5_27-bis fig_5_27 fig_5_29

all : fig_5_03 fig_5_04 fig_5_08 fig_5_14 fig_5_19 fig_5_20 fig_5_20_cacheAligned fig_5_22 fig_5_23 fig_5_24 fig_5_25 fig_5_26 fig_5_27-bis fig_5_27 fig_5_29

fig_5_03 : 
	$(CXX) $(CXXFLAGS) -o fig_5_03 fig_5_03.cpp $(LIBS)

fig_5_04 : 
	$(CXX) $(CXXFLAGS) -o fig_5_04 fig_5_04.cpp $(LIBS)

fig_5_08 : 
	$(CXX) $(CXXFLAGS) -o fig_5_08 fig_5_08.cpp $(LIBS)

fig_5_14 : 
	$(CXX) $(CXXFLAGS) -o fig_5_14 fig_5_14.cpp $(LIBS)

fig_5_19 : 
	$(CXX) $(CXXFLAGS) -o fig_5_19 fig_5_19.cpp $(LIBS)

fig_5_20 : 
	$(CXX) $(CXXFLAGS) -o fig_5_20 fig_5_20.cpp $(LIBS)

fig_5_20_cacheAligned : 
	$(CXX) $(CXXFLAGS) -o fig_5_20_cacheAligned fig_5_20_cacheAligned.cpp $(LIBS)

fig_5_22 : 
	$(CXX) $(CXXFLAGS) -o fig_5_22 fig_5_22.cpp $(LIBS)

fig_5_23 : 
	$(CXX) $(CXXFLAGS) -o fig_5_23 fig_5_23.cpp $(LIBS)

fig_5_24 : 
	$(CXX) $(CXXFLAGS) -o fig_5_24 fig_5_24.cpp $(LIBS)

fig_5_25 : 
	$(CXX) $(CXXFLAGS) -o fig_5_25 fig_5_25.cpp $(LIBS)

fig_5_26 : 
	$(CXX) $(CXXFLAGS) -o fig_5_26 fig_5_26.cpp $(LIBS)

fig_5_27-bis : 
	$(CXX) $(CXXFLAGS) -o fig_5_27-bis fig_5_27-bis.cpp $(LIBS)

fig_5_27 : 
	$(CXX) $(CXXFLAGS) -o fig_5_27 fig_5_27.cpp $(LIBS)

fig_5_29 : 
	$(CXX) $(CXXFLAGS) -o fig_5_29 fig_5_29.cpp $(LIBS)

clean : 
	rm -f fig_5_03 fig_5_04 fig_5_08 fig_5_14 fig_5_19 fig_5_20 fig_5_20_cacheAligned fig_5_22 fig_5_23 fig_5_24 fig_5_25 fig_5_26 fig_5_27-bis fig_5_27 fig_5_29 fig_5_03.o fig_5_04.o fig_5_08.o fig_5_14.o fig_5_19.o fig_5_20.o fig_5_20_cacheAligned.o fig_5_22.o fig_5_23.o fig_5_24.o fig_5_25.o fig_5_26.o fig_5_27-bis.o fig_5_27.o fig_5_29.o *.bmp *.txt out*.png


# \
!endif
