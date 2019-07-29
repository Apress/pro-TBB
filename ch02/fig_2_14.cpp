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
#include <vector>
#include <tbb/tbb.h>

int fig_2_14(const std::vector<int>& v, std::vector<int>& rsum) {
  int N = v.size();
  rsum[0] = v[0];
  int final_sum = tbb::parallel_scan(
    /* range = */ tbb::blocked_range<int>(1, N), 
    /* identity = */ (int)0,
    /* scan body */ 
    [&v, &rsum](const tbb::blocked_range<int>& r, 
                int sum, bool is_final_scan) -> int {
      for (int i = r.begin(); i < r.end(); ++i) {
        sum += v[i];
        if (is_final_scan) 
          rsum[i] = sum;
      }
      return sum;
    },
    /* combine body */
    [](int x, int y) {
      return x + y;
    }
  );
  return final_sum;
}

int serialImpl(const std::vector<int>& v, std::vector<int>& rsum) {
  int N = v.size();
  rsum[0] = v[0];
  for (int i = 1; i < N; ++i) {
    rsum[i] = rsum[i-1] + v[i];
  }
  int final_sum = rsum[N-1];
  return final_sum;
}

static void warmupTBB() {
  tbb::parallel_for(0, tbb::task_scheduler_init::default_num_threads(), [](int) {
    tbb::tick_count t0 = tbb::tick_count::now();
    while ((tbb::tick_count::now() - t0).seconds() < 0.01);
  });
}

int main() {
  const int N = 1e4;
  std::vector<int> v(N, 0);
  std::vector<int> serial_rsum(N, 0);
  std::vector<int> parallel_rsum(N, 0);
  for (int i = 0; i < N; ++i) {
    v[i] = i;
  }

  double serial_time = 0.0, parallel_time = 0.0;
  {
    tbb::tick_count t0 = tbb::tick_count::now();
    int final_sum = serialImpl(v, serial_rsum);
    serial_time = (tbb::tick_count::now() - t0).seconds();
    if (final_sum != N*(N-1)/2) {
      std::cerr << "ERROR: serial final_sum is wrong! " 
                << final_sum << " != " << N*(N-1)/2 
                << std::endl;
    }
  }

  warmupTBB();
  {
    tbb::tick_count t0 = tbb::tick_count::now();
    int final_sum = fig_2_14(v, parallel_rsum);
    parallel_time = (tbb::tick_count::now() - t0).seconds();
    if (final_sum != N*(N-1)/2) {
      std::cerr << "ERROR: parallel final_sum is wrong! " 
                << final_sum << " != " << N*(N-1)/2 
                << std::endl;
    }
  }
 
  if (serial_rsum != parallel_rsum) {
    std::cerr << "ERROR: rsum vectors do not match!" << std::endl;
  }

  std::cout << "serial_time == " << serial_time << " seconds" << std::endl
            << "parallel_time == " << parallel_time << " seconds" << std::endl
            << "speedup == " << serial_time/parallel_time << std::endl;

  return 0;
}

