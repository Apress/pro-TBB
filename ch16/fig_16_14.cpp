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

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <tbb/tbb.h>

template <typename Partitioner>
void fig_16_14(double v, int N, double *a, const Partitioner& p) {
  tbb::parallel_for( tbb::blocked_range<int>(0, N, 1), 
    [v, a](const tbb::blocked_range<int>& r) {
      int ie = r.end();
      for (int i = r.begin(); i < ie; ++i) {
        a[i] += v;
      }
    }, p
  );
}

template <typename Partitioner>
void fig_16_14(double v, int N, double *a, Partitioner& p) {
  tbb::parallel_for( tbb::blocked_range<int>(0, N, 1), 
    [v, a](const tbb::blocked_range<int>& r) {
      int ie = r.end();
      for (int i = r.begin(); i < ie; ++i) {
        a[i] += v;
      }
    }, p
  );
}

void resetV(int N, double *v) {
  for (int i = 0; i < N; ++i) {
    v[i] = i;
  }
  std::random_shuffle(v, v+N);
}

void resetA(int N, double *a) {
  for (int i = 0; i < N; ++i) {
    a[i] = 0;
  }
}

static void warmupTBB() {
  tbb::parallel_for(0, tbb::task_scheduler_init::default_num_threads(), [](int) {
    tbb::tick_count t0 = tbb::tick_count::now();
    while ((tbb::tick_count::now() - t0).seconds() < 0.01);
  });
}

int main(int argc, char *argv[]) {
  int M = 10000;
  int N = 100000;

  std::cout << "P = " << tbb::task_scheduler_init::default_num_threads() 
            << std::endl << "N = " << N 
            << std::endl << "M = " << M << std::endl;

   double *v = new double[M];
   double *a = new double[N]; 

   warmupTBB();
   resetV(M, v);
   resetA(N, a);
   tbb::tick_count t0 = tbb::tick_count::now();
   for (int i = 0; i < M; ++i) {
     fig_16_14(v[i], N, a, tbb::auto_partitioner{});
   }
   double auto_time = (tbb::tick_count::now() - t0).seconds();

   warmupTBB();
   resetA(N, a);
   tbb::affinity_partitioner aff_p;
   t0 = tbb::tick_count::now();
   for (int i = 0; i < M; ++i) {
     fig_16_14(v[i], N, a, aff_p); 
   }
   double affinity_time = (tbb::tick_count::now() - t0).seconds();

   warmupTBB();
   resetA(N, a);
   t0 = tbb::tick_count::now();
   for (int i = 0; i < M; ++i) {
     fig_16_14(v[i], N, a, tbb::static_partitioner{});
  }
  double static_time = (tbb::tick_count::now() - t0).seconds();

  std::cout << "auto_partitioner = " << auto_time << std::endl
            << "affinity_partitioner = " << affinity_time << std::endl
            << "static_partitioner = " << static_time << std::endl;

  delete [] v;
  delete [] a;
  return 0;
}

