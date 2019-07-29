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
#include <tbb/tbb.h>

void doWork(double sec);

template <typename Partitioner>
void fig_16_16(int N, const Partitioner& p) {
  tbb::parallel_for( tbb::blocked_range<int>(0, N, 1), 
    [](const tbb::blocked_range<int>& r) {
      int ie = r.end();
      for (int i = r.begin(); i < ie; ++i) {
        doWork(i);
      }
    }, p
  );
}

template <typename Partitioner>
void fig_16_16(int N, Partitioner& p) {
  tbb::parallel_for( tbb::blocked_range<int>(0, N, 1), 
    [](const tbb::blocked_range<int>& r) {
      int ie = r.end();
      for (int i = r.begin(); i < ie; ++i) {
        doWork(i);
      }
    }, p
  );
}

static void warmupTBB() {
  tbb::parallel_for(0, tbb::task_scheduler_init::default_num_threads(), [](int) {
    tbb::tick_count t0 = tbb::tick_count::now();
    while ((tbb::tick_count::now() - t0).seconds() < 0.01);
  });
}

void doWork(double usec) {
  double sec = usec*1e-06;
  tbb::tick_count t0 = tbb::tick_count::now();
  while ((tbb::tick_count::now() - t0).seconds() <= sec);
}

int main(int argc, char *argv[]) {
  int N = 1000;
  int M = 10;

  std::cout << "P = " << tbb::task_scheduler_init::default_num_threads() 
            << std::endl << "M = " << M
            << std::endl << "N = " << N << std::endl;

   warmupTBB();
   tbb::tick_count t0 = tbb::tick_count::now();
   for (int i = 0; i < M; ++i) {
     fig_16_16(N, tbb::auto_partitioner{});
   }
   double auto_time = (tbb::tick_count::now() - t0).seconds();

   warmupTBB();
   tbb::affinity_partitioner aff_p;
   t0 = tbb::tick_count::now();
   for (int i = 0; i < M; ++i) {
     fig_16_16(N, aff_p); 
   }
   double affinity_time = (tbb::tick_count::now() - t0).seconds();

   warmupTBB();
   t0 = tbb::tick_count::now();
   for (int i = 0; i < M; ++i) {
     fig_16_16(N, tbb::static_partitioner{});
  }
  double static_time = (tbb::tick_count::now() - t0).seconds();

  std::cout << "auto_partitioner = " << auto_time << " seconds" << std::endl
            << "affinity_partitioner = " << affinity_time << " seconds" << std::endl
            << "static_partitioner = " << static_time << " seconds" << std::endl;
  return 0;
}

