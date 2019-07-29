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

#include <iostream>
#include <string>
#include <tbb/tbb.h>

void fig_16_6(int N, double *a, double *b) {
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      b[j*N+i] = a[i*N+j];
    }
  }
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
   setArray(N, a);
   setArray(N, b);

   fig_16_6(N, a, b);
   tbb::tick_count t0 = tbb::tick_count::now();
   fig_16_6(N, a, b);
   double ts = (tbb::tick_count::now()-t0).seconds();   
   checkTranspose(N, b);
   std::cout << "Serial Time = " << ts << std::endl;
   delete [] a;
   delete [] b;
   return 0;
}

