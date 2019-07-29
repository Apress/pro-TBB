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

void doWork();

const int N = 128;

void fig_12_11(int M) {
  tbb::parallel_for(0, M,
    [](int) {
      tbb::parallel_for(0, N, [](int j) {doWork();});
    }
  );
}

tbb::atomic<int> count;

void doWork() {
  ++count;
  tbb::tick_count t0 = tbb::tick_count::now();
  while ((tbb::tick_count::now() - t0).seconds() < 1e-6);
}

static void warmupTBB() {
  tbb::parallel_for(0, tbb::task_scheduler_init::default_num_threads(), [](int) {
    tbb::tick_count t0 = tbb::tick_count::now();
    while ((tbb::tick_count::now() - t0).seconds() < 0.01);
  });
}

int main() {
  const int P = tbb::task_scheduler_init::default_num_threads();
  double total_time_P = 0.0;
  const int L = 100000;
  double total_time_L = 0.0;

  count = 0;
  warmupTBB();
  {
    tbb::tick_count t0 = tbb::tick_count::now();
    fig_12_11(P);
    total_time_P = (tbb::tick_count::now() -t0).seconds();
  }
  {
    tbb::tick_count t0 = tbb::tick_count::now();
    fig_12_11(L);
    total_time_L = (tbb::tick_count::now() -t0).seconds();
  }

  int final_count = count;
  if (final_count != (P+L)*N) {
    std::cout << "ERROR: incorrect amount of work done." << std::endl;
  }
  std::cout << "Time for " << P << " arenas == " << total_time_P << " seconds" << std::endl
            << "Time for " << L << " arenas == " << total_time_L << " seconds" << std::endl;
  return 0;
}

