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
// NOTE: low-level task API REMOVED in oneTBB
// Use task_group instead:
#include <tbb/task_group.h>
#include <tbb/tick_count.h>
// NOTE: task_scheduler_init REMOVED in oneTBB
// Use global_control instead:
#include <tbb/global_control.h>

constexpr int cutoff = 30;

long fib(long n) {
  if(n<2)
    return n;
  else
    return fib(n-1)+fib(n-2);
}

class FibCont {
public:
  long* const sum;
  long x, y;
  FibCont(long* sum_) : sum{sum_} {}
  void operator()() const {
    *sum = x+y;
    return;
  }
};

class FibTask {
public:
  long const n;
  long* const sum;
  
  FibTask(long n_, long* sum_) : n{n_}, sum{sum_} {}
  void operator()() const { // Overrides virtual function task::execute
    if(n<cutoff) {
      *sum = fib(n);
    }
    else {
      long x = 0, y = 0;
      // New task_group so that you can wait for it
      tbb::task_group tg;
      tg.run(FibTask{n-1, &x});
      tg.run(FibTask{n-2, &y});
      // Wait for both children.
      tg.wait();
      // Do the sum
      tg.run(FibCont(sum));
      tg.wait();
    }
    return;
  }
};

long parallel_fib(long n) {
  long sum = 0;
  tbb::task_group tg;
  tg.run(FibTask{n, &sum});
  tg.wait();
  return sum;
}

int main(int argc, char** argv)
{
int n = 40;
size_t nth = 8;

tbb::global_control global_limit{tbb::global_control::max_allowed_parallelism, nth};

auto t0 = tbb::tick_count::now();
long fib_s = fib(n);
auto t1 = tbb::tick_count::now();
long fib_p = parallel_fib(n);
auto t2 = tbb::tick_count::now();
double t_s = (t1 - t0).seconds();
double t_p = (t2 - t1).seconds();

std::cout << "SerialFib:   " << fib_s << " Time: " << t_s << "\n";
std::cout << "ParallelFib: " << fib_p << " Time: " << t_p << " Speedup: " << t_s/t_p << "\n";
return 0;
}
//g++ -o f fig_10_11.cpp -std=c++11 -O3 -ltbb -Wall -pedantic -ltbbmalloc_proxy
// SerialFib:   102334155 Time: 0.522419
// ParallelFib: 102334155 Time: 0.134931 Speedup: 3.87175
