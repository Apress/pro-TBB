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
#include <tbb/tbb.h>

tbb::spin_mutex mylock;
int counter = 0;
int values[] = { 11,22,33,44,55,66,77,88,99,42 };

int main( int argc, char *argv[] ) {
  int howmany =
    parallel_deterministic_reduce(
      tbb::blocked_range<int*>( values, values+10 ),
      0,
      [](const tbb::blocked_range<int*>& r, int init)->int {
	int rr = init;
	for( int* a=r.begin(); a!=r.end(); ++a ) {
	  int mycount;
	  { tbb::spin_mutex::scoped_lock hello(mylock);
	    mycount = counter++;
	  }
	  printf("Hello, World (%02d):(%02d)\n",*a,mycount);
	  rr += *a;
	} /* end of lambda */
	return rr;
      },
      []( int x, int y )->int {
	return x+y;
      }
    );
  printf("Hello, parallel_deterministic_reduce(%02d)\n",howmany);
  return 0;
}
