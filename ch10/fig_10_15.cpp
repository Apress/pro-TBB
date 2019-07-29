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
#include <tbb/task.h>
#include <tbb/tick_count.h>
#include <tbb/task_scheduler_init.h>

int cutoff = 30;

long fib(long n) {
  if(n<2)
    return n;
  else
    return fib(n-1)+fib(n-2);
}

class FibCont: public tbb::task {
public:
  long* const sum;
  long x, y;
  FibCont(long* sum_) : sum{sum_} {}
  tbb::task* execute(){
    *sum = x+y;
    return nullptr;
  }
};

class FibTask: public tbb::task {
public:
  long n;    // not const anymore
  long* sum; // not const ptr anymore
  FibTask(long n_, long* sum_) : n{n_}, sum{sum_} {}
  tbb::task* execute() { // Overrides virtual function task::execute
    if(n<cutoff) {
      *sum = fib(n);
      return nullptr;
    }
    else {
      // long x, y; not needed anymore
      FibCont& c = *new(allocate_continuation()) FibCont{sum};
      FibTask& b = *new(c.allocate_child()) FibTask{n-2, &c.y};
      recycle_as_child_of(c);
      this->n -=1;
      this->sum = &c.x;
      // Set ref_count to "two children".
      c.set_ref_count(2);
      tbb::task::spawn(b);
      //spawn (*this); return NULL; //This is an alternative
      return this;
    }
  }
};

long parallel_fib(long n) {
  long sum = 0;
  FibTask& a = *new(tbb::task::allocate_root()) FibTask{n,&sum};
  tbb::task::spawn_root_and_wait(a);
  return sum;
}

int main(int argc, char** argv)
{
int n = 30;
int nth = 4;

tbb::task_scheduler_init init{nth};

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
//g++ -o f fig_10_15.cpp -std=c++11 -O3 -ltbb -Wall -pedantic -ltbbmalloc_proxy
//SerialFib:   102334155 Time: 0.523446
//ParallelFib: 102334155 Time: 0.133716 Speedup: 3.91461
