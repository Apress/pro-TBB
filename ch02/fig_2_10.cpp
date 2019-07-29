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

// avoid Windows macros
#define NOMINMAX

#include <iostream>
#include <limits>
#include <tbb/tbb.h>

int fig_2_10(const std::vector<int>& a) {
  int max_value = tbb::parallel_reduce(
    /* the range = */ tbb::blocked_range<int>(0, a.size()), 
    /* identity = */ std::numeric_limits<int>::min(),
    /* func = */ 
    [&](const tbb::blocked_range<int>& r, int init) -> int {
      for (int i = r.begin(); i != r.end(); ++i) {
        init = std::max(init, a[i]);
      }
      return init;
    },
    /* reduction = */ 
    [](int x, int y) -> int {
      return std::max(x,y);
    }
  );
  return max_value;
}

int serialImpl(const std::vector<int>& a) {
  int max_value = std::numeric_limits<int>::min();
  for (int i = 0; i < a.size(); ++i) {
    max_value = std::max(max_value,a[i]);
  }
  return max_value;
}

static void warmupTBB() {
  tbb::parallel_for(0, tbb::task_scheduler_init::default_num_threads(), [](int) {
    tbb::tick_count t0 = tbb::tick_count::now();
    while ((tbb::tick_count::now() - t0).seconds() < 0.01);
  });
}

int main() {
  const int N = INT_MAX;
  std::vector<int> a(N, 0);
  for (int i = 0; i < N; ++i) {
    a[i] = std::abs(i - N/2);
  }

  double serial_time = 0.0, parallel_time = 0.0;
  {
    tbb::tick_count t0 = tbb::tick_count::now();
    int max_value = serialImpl(a);
    serial_time = (tbb::tick_count::now() - t0).seconds();
    if (max_value != N/2) {
      std::cerr << "ERROR: serial max_value is wrong!" << std::endl;
    }
  }

  warmupTBB();
  {
    tbb::tick_count t0 = tbb::tick_count::now();
    int max_value = fig_2_10(a);
    parallel_time = (tbb::tick_count::now() - t0).seconds();
    if (max_value != N/2) {
      std::cerr << "ERROR: parallel max_value is wrong!" << std::endl;
    }
  }

  std::cout << "serial_time == " << serial_time << " seconds" << std::endl
            << "parallel_time == " << parallel_time << " seconds" << std::endl
            << "speedup == " << serial_time/parallel_time << std::endl;

  return 0;
}

