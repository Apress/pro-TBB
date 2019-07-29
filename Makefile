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

.PHONY : all clean ch01 ch02 ch03 ch04 ch05 ch06 ch07 ch10 ch11 ch12 ch13 ch14 ch15 ch16 ch17 ch18 ch19 ch20 AppB

# \
!ifndef 0   # \
all clean:  _printmsg # \
# \
_printmsg:  # \
	@echo "for nmake, please specify /f Makefile.nmake"
# \
!else

all : ch01 ch02 ch03 ch04 ch05 ch06 ch07 ch10 ch11 ch12 ch13 ch14 ch15 ch16 ch17 ch18 ch19 ch20 AppB

ch01 :
	(cd ch01; $(MAKE))

ch02 :
	(cd ch02; $(MAKE))

ch03 :
	(cd ch03; $(MAKE))

ch04 :
	(cd ch04; $(MAKE))

ch05 :
	(cd ch05; $(MAKE))

ch06 :
	(cd ch06; $(MAKE))

ch07 :
	(cd ch07; $(MAKE))

ch10 :
	(cd ch10; $(MAKE))

ch11 :
	(cd ch11; $(MAKE))

ch12 :
	(cd ch12; $(MAKE))

ch13 :
	(cd ch13; $(MAKE))

ch14 :
	(cd ch14; $(MAKE))

ch15 :
	(cd ch15; $(MAKE))

ch16 :
	(cd ch16; $(MAKE))

ch17 :
	(cd ch17; $(MAKE))

ch18 :
	(cd ch18; $(MAKE))

ch19 :
	(cd ch19; $(MAKE))

ch20 :
	(cd ch20; $(MAKE))

AppB :
	(cd AppB; $(MAKE))

clean :
	(cd ch01; $(MAKE) clean)
	(cd ch02; $(MAKE) clean)
	(cd ch03; $(MAKE) clean)
	(cd ch04; $(MAKE) clean)
	(cd ch05; $(MAKE) clean)
	(cd ch06; $(MAKE) clean)
	(cd ch07; $(MAKE) clean)
	(cd ch10; $(MAKE) clean)
	(cd ch11; $(MAKE) clean)
	(cd ch12; $(MAKE) clean)
	(cd ch13; $(MAKE) clean)
	(cd ch14; $(MAKE) clean)
	(cd ch15; $(MAKE) clean)
	(cd ch16; $(MAKE) clean)
	(cd ch17; $(MAKE) clean)
	(cd ch18; $(MAKE) clean)
	(cd ch19; $(MAKE) clean)
	(cd ch20; $(MAKE) clean)
	(cd AppB; $(MAKE) clean)
# \
!endif
