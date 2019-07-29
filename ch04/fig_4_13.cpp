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
#include <limits>
#include <math.h>
#include <tbb/tbb.h>
#include <pstl/execution>
#include <pstl/algorithm>
#include <pstl/numeric>

//
// For best performance when using the Intel compiler use
// EXTRA_CXXFLAGS="-qopenmp-simd -xHost" when building
//

//const int num_intervals = std::numeric_limits<int>::max();
const int num_intervals = 1<<22;

float fig_4_13() {
  constexpr const float dx = 1.0 / num_intervals;
  float sum = std::transform_reduce(
    /* policy */ pstl::execution::par_unseq,
    /* first */ tbb::counting_iterator<int>(0),
    /* last */  tbb::counting_iterator<int>(num_intervals),
    /* init = */ 0.0,
    /* reduce */
    [](float x, float y) -> float {
      return x + y;
    },
    /* transform */
    [=](int i) -> float {
      float x = (i + 0.5)*dx;
      float h = sqrt(1 - x*x);
      return h*dx;
    }
  );
  return 4 * sum;
}

void warmupTBB() {
  tbb::parallel_for(0, tbb::task_scheduler_init::default_num_threads(), [](int) {
    tbb::tick_count t0 = tbb::tick_count::now();
    while ((tbb::tick_count::now() - t0).seconds() < 0.001);
  });
}

void run_fig_4_13() {
  double version_time = 0.0;
  warmupTBB();
  {
    tbb::tick_count t0 = tbb::tick_count::now();
    auto pi = fig_4_13();
    version_time = (tbb::tick_count::now() - t0).seconds();
    std::cout << "pi with par_unseq == " << pi << std::endl;
  }
  std::cout << "time == " << version_time << " seconds for par_unseq" << std::endl;
}

template <typename Policy>
float pi_template(const Policy& p) {
  constexpr const float dx = 1.0 / num_intervals;
  float sum = std::transform_reduce(
    /* policy */ p,
    /* first */ tbb::counting_iterator<int>(0),
    /* last */  tbb::counting_iterator<int>(num_intervals),
    /* init = */ 0.0,
    /* reduce */
    [](float x, float y) -> float {
      return x + y;
    },
    /* transform */
    [=](int i) -> float {
      float x = (i + 0.5)*dx;
      float h = sqrt(1 - x*x);
      return h*dx;
    }
  );
  return 4 * sum;
}

template <typename Policy>
void run_pi_template(const Policy& p, const std::string& name) {

  double version_time = 0.0;
  warmupTBB();
  {
    tbb::tick_count t0 = tbb::tick_count::now();
    auto pi = pi_template(p);
    version_time = (tbb::tick_count::now() - t0).seconds();
    std::cout << "pi with " << name << " == " << pi << std::endl;
  }
  std::cout << "time == " << version_time << " seconds for " << name << std::endl;
}

int main() {
  run_pi_template(pstl::execution::seq, "seq");
  run_pi_template(pstl::execution::unseq, "unseq");
  run_pi_template(pstl::execution::par, "par");
  run_pi_template(pstl::execution::par_unseq, "par_unseq");
  run_fig_4_13();
  std::cout << "Done." << std::endl;
  return 0;
}

