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
#include <pstl/execution>
#include <pstl/algorithm>

//
// For best performance when using the Intel compiler use
// EXTRA_CXXFLAGS="-qopenmp-simd -xHost" when building

static void warmupTBB();
void accumulateTime(tbb::tick_count& t0, int version);
void validateResults(int num_trials, const std::vector<float>& v);
void dumpTimes();

void fig_4_10() {
  const int num_trials = 1000;
  const int n = 65536;
  tbb::tick_count t0;

  std::vector<float> a(n, 1.0), b(n, 3.0);

  for (int t = 0; t < num_trials; ++t) {
    warmupTBB();
    t0 = tbb::tick_count::now();
    std::transform(pstl::execution::par,
      /* in1 range */ a.begin(), a.end(),
      /* in2 first */ b.begin(), 
      /* out first */ a.begin(),
      [](float ae, float be) -> float {
        return ae + be*be;
      }
    );
    accumulateTime(t0, 4);
    std::transform(pstl::execution::par_unseq,
      /* in1 range */ a.begin(), a.end(),
      /* in2 first */ b.begin(), 
      /* out first */ a.begin(),
      [](float ae, float be) -> float {
        return ae + be*be;
      }
    );
    accumulateTime(t0, 5);
    tbb::parallel_for(0, n,
      [&a, &b](int i) {
        a[i] = a[i] + b[i]*b[i];
      }
    );
    accumulateTime(t0, 6);
#pragma novector
    for (int i = 0; i < n; ++i) {
      a[i] = a[i] + b[i]*b[i];
    }
    accumulateTime(t0, 0);
    std::transform(
      /* in1 range */ a.begin(), a.end(),
      /* in2 first */ b.begin(),
      /* out first */ a.begin(),
      [](float ae, float be) -> float {
        return ae + be*be;
      }
    );
    accumulateTime(t0, 1);
    std::transform(pstl::execution::seq,
      /* in1 range */ a.begin(), a.end(),
      /* in2 first */ b.begin(), 
      /* out first */ a.begin(),
      [](float ae, float be) -> float {
        return ae + be*be;
      }
    );
    accumulateTime(t0, 2);
    std::transform(pstl::execution::unseq,
      /* in1 range */ a.begin(), a.end(),
      /* in2 first */ b.begin(), 
      /* out first */ a.begin(),
      [](float ae, float be) -> float {
        return ae + be*be;
      }
    );
    accumulateTime(t0, 3);
  }
  validateResults(num_trials, a);
  dumpTimes();
}

const int num_versions = 7;
double total_times[num_versions] = {0,0,0,0,0,0,0};

void accumulateTime(tbb::tick_count& t0, int version) {
  if (version >= 0) {
    double elapsed_time = (tbb::tick_count::now()-t0).seconds(); 
    total_times[version] += elapsed_time;
    t0 = tbb::tick_count::now();
  }
}

void dumpTimes() {
  const char *versions[num_versions] = 
    { "for", "none", "seq", "unseq", "par", "par_unseq", "pfor" }; 
 
  for (int i = 0; i < num_versions; ++i) {
    std::cout << versions[i] << ", " << total_times[i] << std::endl;
    total_times[i] = 0.0;
  }
}

void validateResults(int num_trials, const std::vector<float>& a) {
   float r = num_trials * num_versions * 9 + 1;
   for (auto& i : a ) {
     if (r != i) {
       std::cout << "ERROR: results did not match" << std::endl;
       return;
     }
   }
}

static void warmupTBB() {
  tbb::parallel_for(0, tbb::task_scheduler_init::default_num_threads(), [](int) {
    tbb::tick_count t0 = tbb::tick_count::now();
    while ((tbb::tick_count::now() - t0).seconds() < 0.01);
  });
}

int main() {
  double  total_time = 0.0;
  {
    tbb::tick_count t0 = tbb::tick_count::now();
    fig_4_10(); 
    total_time = (tbb::tick_count::now() - t0).seconds();
  }
  std::cout << "total_time == " << total_time << " seconds" << std::endl;
  return 0;
}

