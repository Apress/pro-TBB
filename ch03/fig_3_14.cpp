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

void fig_3_14(std::vector<double>& x, const std::vector<double>& a, 
              std::vector<double>& b) {
  const int N = x.size();
  for (int i = 0; i < N; ++i) {
    b[i] -= tbb::parallel_reduce(tbb::blocked_range<int>{0,i}, 0.0,
      [&a, &x, i, N] (const tbb::blocked_range<int>& b, double init) -> double {
        for (int j = b.begin(); j < b.end(); ++j) {
          init += a[j + i*N] * x[j];
        }
        return init;
      },
      std::plus<double>() 
    );
    x[i] = b[i] / a[i + i*N];
  }
}

static std::vector<double> initForwardSubstitution(std::vector<double>& x, 
                                                   std::vector<double>& a, 
                                                   std::vector<double>& b) {
  const int N = x.size();
  for (int i = 0; i < N; ++i) {
    x[i] = 0;
    b[i] = i*i;
    for (int j = 0; j <= i; ++j) {
      a[j + i*N] = 1 + j*i;
    }
  }

  std::vector<double> b_tmp = b;
  std::vector<double> x_gold = x;
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < i; ++j) {
      b_tmp[i] -= a[j + i*N] * x_gold[j];
    }
    x_gold[i] = b_tmp[i] / a[i + i*N];
  }
  return x_gold;
}

static void warmupTBB() {
  tbb::parallel_for(0, tbb::task_scheduler_init::default_num_threads(), [](int) {
    tbb::tick_count t0 = tbb::tick_count::now();
    while ((tbb::tick_count::now() - t0).seconds() < 0.01);
  });
}

int main() {
  const int N = 32768;

  std::vector<double> a(N*N);
  std::vector<double> b(N);
  std::vector<double> x(N);

  auto x_gold = initForwardSubstitution(x,a,b);

  warmupTBB();
  double parallel_time = 0.0;
  {
    tbb::tick_count t0 = tbb::tick_count::now();
    fig_3_14(x,a,b);
    parallel_time = (tbb::tick_count::now() - t0).seconds();
  }
  for (int i = 0; i < N; ++i) {
    if (x[i] > 1.1*x_gold[i] || x[i] < 0.9*x_gold[i]) {
        std::cerr << "  at " << i << " " << x[i] << " != " << x_gold[i] << std::endl;
    }
  }
  std::cout << "parallel_reduce_time == " << parallel_time << " seconds" << std::endl;
  return 0;
}

