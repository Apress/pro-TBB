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

#include <vector>
#include <tbb/tbb.h>

void f(int v);

void fig_2_6(int N, const std::vector<int>& a) {
  tbb::parallel_for(0, N, 1, [a](int i) {
    f(a[i]);
  });
}

void serialImpl(int N, const std::vector<int>& a) {
  for (int i = 0; i < N; ++i) {
    f(a[i]);
  }
}

void spinWaitForAtLeast(double sec) {
  if (sec == 0.0) return;
  tbb::tick_count t0 = tbb::tick_count::now();
  while ((tbb::tick_count::now() - t0).seconds() < sec);
}

void f(int v) {
  if (v%2) {
    spinWaitForAtLeast(0.001);
  } else {
    spinWaitForAtLeast(0.002);
  }
}

static void warmupTBB() {
  tbb::parallel_for(0, tbb::task_scheduler_init::default_num_threads(), [](int) {
    tbb::tick_count t0 = tbb::tick_count::now();
    while ((tbb::tick_count::now() - t0).seconds() < 0.01);
  });
}

#include <algorithm>
#include <iostream>

int main() {
  const int N = 1000;

  std::vector<int> v(N, 0);
  int i = 0;
  std::generate(v.begin(), v.end(), [&i]() { return i++; });

  double serial_time = 0.0, parallel_time = 0.0;
  {
    tbb::tick_count t0 = tbb::tick_count::now();
    serialImpl(N, v);
    serial_time = (tbb::tick_count::now() - t0).seconds();
  }

  warmupTBB();
  {
    tbb::tick_count t0 = tbb::tick_count::now();
    fig_2_6(N, v);
    parallel_time = (tbb::tick_count::now() - t0).seconds();
  }

  std::cout << "serial_time == " << serial_time << " seconds" << std::endl
            << "parallel_time == " << parallel_time << " seconds" << std::endl
            << "speedup == " << serial_time/parallel_time << std::endl;

  return 0;
}

