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

/*
Hello, Sorted myvalues:  01 02 03 04 05 06 07 08 09 10
Hello, Sorted myarray:   11 12 13 14 15 16 17 18 19 20
Hello, Sorted disarray:  21 22 23 24 25 26 27 28 29 30
Hello, Sorted myvect:    31 32 33 34 34 35 36 37 38 40
*/
#include <cstdio>
#include <tbb/tbb.h>
#include <array>

#define PV(X) printf(" %02d", X);
#define PN(Y) printf( "\nHello, Sorted " #Y ":\t");
#define P(N) PN(N); std::for_each(N.begin(),N.end(),[](int x) { PV(x); });
#define V(Z) myvect.push_back(Z);

int main( int argc, char *argv[] ) {
  int myvalues[]                = {  3,  9,  4,  5,  1,  7,  6,  8, 10,  2 };
  std::array<int, 10> myarray   = { 19, 13, 14, 11, 15, 20, 17, 16, 12, 18 };
  std::array<int, 10> disarray  = { 23, 29, 27, 25, 30, 21, 26, 24, 28, 22 };
  tbb::concurrent_vector<int> myvect;
  V(40); V(31); V(37); V(33); V(34); V(32); V(34); V(35); V(38); V(36);

  tbb::parallel_sort( myvalues,myvalues+10 );
  tbb::parallel_sort( myarray.begin(), myarray.end() );
  tbb::parallel_sort( disarray );
  tbb::parallel_sort( myvect );

  PN(myvalues); for(int i=0;i<10;i++) PV(myvalues[i]);
  P(myarray);
  P(disarray);
  P(myvect);
  printf("\n\n");
  return 0;
}
