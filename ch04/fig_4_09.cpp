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
#include <pstl/execution>
#include <pstl/algorithm>

//
// For best performance when using the Intel compiler use
// EXTRA_CXXFLAGS="-qopenmp-simd -xHost" when building
//

inline void fig_4_9(float * a, float * b, float * c) {
  const int M = 1024;
  std::for_each(
    /* policy */ pstl::execution::par_unseq,
    /* first */  tbb::counting_iterator<int>(0),
    /* last */   tbb::counting_iterator<int>(M),
    [&a, &b, &c, M](int i) {
      for (int j = 0; j < M; ++j) {
        int c_index = i*M + j;
        for (int k = 0; k < M; ++k) {
          c[c_index] += a[i*M + k] * b[k*M + j];
        }
      }
    }
  );
}

void warmupTBB() {
  tbb::parallel_for(0, tbb::task_scheduler_init::default_num_threads(), [](int) {
    tbb::tick_count t0 = tbb::tick_count::now();
    while ((tbb::tick_count::now() - t0).seconds() < 0.001);
  });
}

void run_fig_4_9() {
  const int M = 1024;
  const int MxM = M*M;
  float *a = new float[MxM];
  float *b = new float[MxM];
  float *c = new float[MxM];
  float *c_expected = new float[MxM];

  // init arrays
  std::fill(a, a + MxM, 1.0);
  std::fill(b, b + MxM, 1.0);
  std::fill(c, c + MxM, 0.0);
  std::fill(c_expected, c_expected + MxM, M);

  double version_time = 0.0;
  warmupTBB();
  {
    tbb::tick_count t0 = tbb::tick_count::now();
    fig_4_9(a, b, c);
    version_time = (tbb::tick_count::now() - t0).seconds();
    if (!std::equal(c, c + MxM, c_expected)) {
      std::cerr << "ERROR: c array does not match expected values for fig_4_9" << std::endl;
    }
  }

  std::cout << "time == " << version_time << " seconds for par_unseq" << std::endl;
  delete [] a;
  delete [] b;
  delete [] c;
  delete [] c_expected;
}

template <int M, typename Policy>
inline void mxm_template(const Policy& p, float * a, float * b, float * c) {
  std::for_each(
    /* policy */ p,
    /* first */ tbb::counting_iterator<int>(0),
    /* last */  tbb::counting_iterator<int>(M),
    [&a, &b, &c](int i) {
#pragma novector
      for (int j = 0; j < M; ++j) {
       int c_index = i*M + j;
#pragma novector
        for (int k = 0; k < M; ++k) {
          c[c_index] += a[i*M + k] * b[k*M + j];
        }
      }
    }
  );
}

template <typename Policy>
void run_mxm_template(const Policy& p, const std::string& name) {
  const int M = 1024;
  const int MxM = M*M;
  float *a = new float[MxM];
  float *b = new float[MxM];
  float *c = new float[MxM];
  float *c_expected = new float[MxM];

  // init arrays
  std::fill(a, a + MxM, 1.0);
  std::fill(b, b + MxM, 1.0);
  std::fill(c, c + MxM, 0.0);
  std::fill(c_expected, c_expected + MxM, M);

  double version_time = 0.0;
  warmupTBB();
  {
    tbb::tick_count t0 = tbb::tick_count::now();
    mxm_template<1024>(p, a, b, c);
    version_time = (tbb::tick_count::now() - t0).seconds();
    if (!std::equal(c, c + MxM, c_expected)) {
      std::cerr << "ERROR: c array does not match expected values for " << name << std::endl;
    }
  }

  std::cout << "time == " << version_time << " seconds for " << name << std::endl;
}

int main() {
  run_mxm_template(pstl::execution::seq, "seq");
  run_mxm_template(pstl::execution::unseq, "unseq");
  warmupTBB();
  run_mxm_template(pstl::execution::par, "par");
  warmupTBB();
  run_fig_4_9();
  std::cout << "Done." << std::endl;
  return 0;
}

