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

#include <tbb/tbb.h>
#include <iostream>

void obliviousTranspose(int N, int ib, int ie, int jb, int je, double *a, double *b, int gs) {
  int ilen = ie-ib;
  int jlen = je-jb;
  if (ilen > gs ||  jlen > gs) {
     if ( ilen > jlen ) {
       int imid = (ib+ie)/2;
       obliviousTranspose(N, ib, imid, jb, je, a, b, gs);
       obliviousTranspose(N, imid, ie, jb, je, a, b, gs);
     } else {
       int jmid = (jb+je)/2;
       obliviousTranspose(N, ib, ie, jb, jmid, a, b, gs);
       obliviousTranspose(N, ib, ie, jmid, je, a, b, gs);
     }
  } else {
    for (int i = ib; i < ie; ++i) {
      for (int j = jb; j < je; ++j) {
        b[j*N+i] = a[i*N+j];
      }
    }
  }
}

double obliviousTranspose(int N, double *a, double *b, int gs) {
   tbb::tick_count t0 = tbb::tick_count::now();
   obliviousTranspose(N, 0, N, 0, N, a, b, gs);
   tbb::tick_count t1 = tbb::tick_count::now();
   return (t1-t0).seconds();   
}

void setArray(int N, double *a) {
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      a[i*N+j] = i;
    }
  }
}


void checkTranspose(int N, double *a) {
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      if (a[i*N+j] != j) {
        std::cout << "Transpose failed" << std::endl;
      }
    }
  }
}


int main() {
  int N = 2<<13;
  double *a = new double[N*N];
  double *b = new double[N*N];

  std::cout << "grainsize, oblivious time" << std::endl;
  for (int gs = 1; gs < 2048; gs *= 2) {
    setArray(N, a);
    setArray(N, b);
    obliviousTranspose(N, a, b, gs);
    double to = obliviousTranspose(N, a, b, gs);
    checkTranspose(N, b);
    std::cout << gs << ", " << to << std::endl;
  }

  delete [] a;
  delete [] b;
  return 0;
}

