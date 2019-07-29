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

static inline void spinWaitForAtLeast(double sec=0.0) {
  if (sec == 0.0) return;
  tbb::tick_count t0 = tbb::tick_count::now();
  while ((tbb::tick_count::now() - t0).seconds() < sec);
}

static inline double executeFor(int num_trials, int N, double tpi) {
  tbb::tick_count t0;
  for (int t = -1; t < num_trials; ++t) {
    if (!t) t0 = tbb::tick_count::now();
    for (int i = 0; i < N; ++i) {
      spinWaitForAtLeast(tpi);
    } 
  }
  tbb::tick_count t1 = tbb::tick_count::now();
  return (t1 - t0).seconds()/num_trials;
}

template< typename P >
static inline double executePfor(int num_trials, int N,
		                         int gs, P& p, double tpi) {
  tbb::tick_count t0;
  for (int t = -1; t < num_trials; ++t) {
    if (!t) t0 = tbb::tick_count::now();
    tbb::parallel_for (
      tbb::blocked_range<int>{0, N, static_cast<size_t>(gs)},
      [tpi](const tbb::blocked_range<int>& r) {
        int e = r.end();
        for (int i = r.begin(); i < e; ++i) {
          spinWaitForAtLeast(tpi);
        } 
      }, 
      p
    );
  }
  tbb::tick_count t1 = tbb::tick_count::now();
  return (t1 - t0).seconds()/num_trials;
}

void fig_16_04() {
  tbb::auto_partitioner auto_p;
  tbb::simple_partitioner simple_p;
  tbb::static_partitioner static_p;
  const std::string pname[4] = {"simple", "auto", "affinity", "static"};

  const int N = 262144;
  const int T = 20;
  const double ten_ns = 0.00000001;
  const double twenty_us = 0.00002;
  double timing[4][19];

  for (double tpi = ten_ns; tpi < twenty_us; tpi *= 10) { 
    std::cout << "Speedups for " << tpi << " seconds per iteration" << std::endl
              << "partitioner";
    for (int gs = 1, i = 0; gs <= N; gs *= 2, ++i) 
      std::cout << ", " << gs;
    std::cout << std::endl;

    double serial_time = executeFor(T, N, tpi);

    for (int gs = 1, i = 0; gs <= N; gs *= 2, ++i) {
      tbb::affinity_partitioner affinity_p;
      spinWaitForAtLeast(0.001);
      timing[0][i] = executePfor(T, N, gs, simple_p, tpi);
      timing[1][i] = executePfor(T, N, gs, auto_p, tpi);
      timing[2][i] = executePfor(T, N, gs, affinity_p, tpi);
      timing[3][i] = executePfor(T, N, gs, static_p, tpi);
    }
    for (int p = 0; p < 4; ++p) {
      std::cout << pname[p];  
      for (int gs = 1, i = 0; gs <= N; gs *= 2, ++i) 
        std::cout << ", " << serial_time/timing[p][i];
      std::cout << std::endl;
    }
    std::cout << std::endl;
  }
}

int main() {
  fig_16_04();
  return 0;
}

