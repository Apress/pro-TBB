/*
Copyright (C) 2019 Intel Corporation

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom
the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
OR OTHER DEALINGS IN THE SOFTWARE.

SPDX-License-Identifier: MIT
*/

/** Minimal Ten Times Hello World. Created by James Reinders **/

#include <cstdio>
#include <array>
#include <tbb/tbb.h>

tbb::spin_mutex mylock;
int counter = 0;
std::array<int, 10> values{ { 11,22,33,44,55,66,77,88,99,42 } };

int main( int argc, char *argv[] ) {
  tbb::parallel_for_each(
    std::begin(values),
    std::end(values),
    [](int i) {
      int mycount;
      { tbb::spin_mutex::scoped_lock hello(mylock);
	mycount = counter++;
      }
      printf("Hello, World (%02d):(%02d)\n",i,mycount);
    } /* end of lambda */
  ); /* end of parallel_for_each */
  return 0;
}
