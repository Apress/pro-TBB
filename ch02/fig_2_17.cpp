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

#include <cmath>
#include <iostream>
#include <list>
#include <map>
#include <random>
#include <utility>
#include <vector>
#include <tbb/tbb.h>

using PrimesValue = std::pair<int, bool>;
using PrimesList = std::list<PrimesValue>;

bool isPrime(int n);

void fig_2_17(PrimesList& values) {
  tbb::parallel_do(values,
    [](PrimesList::reference v) {
      if (isPrime(v.first)) 
        v.second = true;
    }
  );
}

void serialImpl(PrimesList& values) {
  for (PrimesList::reference v : values) {
    if (isPrime(v.first)) 
      v.second = true;
  }
}

bool isPrime(int n) {
  int e =  std::sqrt(n);
  std::vector<bool> p(e+1, true);

  for (int i = 2; i <= e; ++i) {
    if (p[i]) {
      if (n % i) {
        for (int j = 2*i; j <= e; j += i) {
          p[j] = false;
        }
      } else {
        return false;
      }
    }
  }
  return true;
}

using PrimesMap = std::map<int, bool>;
using IntVector = std::vector<int>;

static IntVector makePrimesValues(int n, PrimesMap& m) {
  std::default_random_engine gen;
  std::uniform_int_distribution<int> dist;
  IntVector vec;

  for (int i = 0; i < n; ++i) {
    int v = dist(gen);
    vec.push_back( v );
    m[v] = isPrime(v);
  }
  return vec;
}

static PrimesList makePrimesList(const IntVector& vec) {
  PrimesList l;
  for (auto& v : vec) {
    l.push_back(PrimesValue(v, false));
  }
  return l;
}

static void warmupTBB() {
  tbb::parallel_for(0, tbb::task_scheduler_init::default_num_threads(), [](int) {
    tbb::tick_count t0 = tbb::tick_count::now();
    while ((tbb::tick_count::now() - t0).seconds() < 0.01);
  });
}

int main() {
  const int levels = 14;
  const int N = std::pow(2, levels) - 1;
  PrimesMap m;
  auto vec = makePrimesValues(N, m);
  PrimesList slist = makePrimesList(vec);
  PrimesList plist = makePrimesList(vec);
  
  double serial_time = 0.0, parallel_time = 0.0;
  {
    tbb::tick_count t0 = tbb::tick_count::now();
    serialImpl(slist);
    serial_time = (tbb::tick_count::now() - t0).seconds();
  }
  for (auto p : slist) {
    if (p.second != m[p.first])
      std::cerr << "Error: serial results are incorrect!" << std::endl;
  }
  
  warmupTBB();
  {
    tbb::tick_count t0 = tbb::tick_count::now();
    fig_2_17(plist);
    parallel_time = (tbb::tick_count::now() - t0).seconds();
  }
  for (auto p : plist) {
    if (p.second != m[p.first])
      std::cerr << "Error: serial results are incorrect!" << std::endl;
  }
  
  if (slist != plist) {
    std::cerr << "Error: serial and parallel implementations do not agree!" << std::endl;
  }
  
  std::cout << "serial_time == " << serial_time << " seconds" << std::endl
            << "parallel_time == " << parallel_time << " seconds" << std::endl
            << "speedup == " << serial_time/parallel_time << std::endl;
  return 0;
}
