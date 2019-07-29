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

#include <cstdio>
#include <tbb/tbb.h>

tbb::spin_mutex mylock;
int counter = 0;
int in[10] = { 1,2,3,4,5,6,7,8,9,10 };
int out[10];

int main( int argc, char *argv[] ) {
  int re = 0;
  re = tbb::parallel_scan(
         tbb::blocked_range<int>(0,10,1),
	 0,
	 [](const tbb::blocked_range<int>& r, int gsum,
          bool is_final_scan)->int {
	   int psum = gsum;
	   int mycount;
	   int tst = 0;
	   for( int i=r.begin(); i<r.end(); ++i ) {
	     psum += in[i];
	     tst++;
	     if ( is_final_scan ) out[i] = psum;
	     { tbb::spin_mutex::scoped_lock hello(mylock);
	       mycount = counter++;
	     }
	   }
	   printf("Hello, World (%02d) %02d+[%02d-%02d)=%02d%c\n",
		  mycount,gsum,r.begin(),r.end(),
		  psum,is_final_scan?'F':'-');
	   return psum;
	 }, /* end of scan lambda */
	 []( int left, int right )->int {
	   printf("Hello, Scan %02d +  %02d\n",left,right);
	   return left + right;
	 } /* end of reverse join lambda */
       ); /* end of parallel_scan */
  printf("Hello, Scanned%4d%4d%4d%4d%4d%4d%4d%4d%4d%4d\n",
	 in[0],in[1],in[2],in[3],in[4],
	 in[5],in[6],in[7],in[8],in[9]);
  printf("     yielding %4d%4d%4d%4d%4d%4d%4d%4d%4d%4d ->%4d\n",
	 out[0],out[1],out[2],out[3],out[4],
	 out[5],out[6],out[7],out[8],out[9],
	 re);
  return 0;
}
