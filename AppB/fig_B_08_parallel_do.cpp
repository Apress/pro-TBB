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
#include <array>

tbb::spin_mutex mylock;
int counter;

#define PN(Y) printf( "\nHello, Do (or Do Not) " #Y ":\n\t");
#define PV(X) {                                         \
          int mycount;                                  \
          { tbb::spin_mutex::scoped_lock hello(mylock); \
            mycount = counter++;                        \
          }                                             \
          printf(" %02d.%d", X, mycount);               \
        }

int main( int argc, char *argv[] ) {
  std::array<int,10> myarray   = { 19,13,14,11,15,20,17,16,12,18 };
  std::array<int,10> disarray  = { 23,29,27,25,30,21,26,24,28,22 };

  counter = 0;
  PN(myarray);
  tbb::parallel_do( myarray.begin(),myarray.end(),
		    [](int xyzzy){ PV(xyzzy); }
		  );
  printf("\n");

  counter = 0;
  PN(disarray);
  tbb::parallel_do( disarray,
		    [](int xyzzy){ PV(xyzzy); }
		  );
  printf("\n\n");
  return 0;
}
