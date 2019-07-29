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
#include <thread>

#include <tbb/tbb.h>

void doWork(double sec) {
  tbb::tick_count t0 = tbb::tick_count::now();
  while ((tbb::tick_count::now() - t0).seconds() < sec);
}

void fig_14_03() {
  std::thread t0([]() {
    tbb::task_group_context tcg;
    tcg.set_priority(tbb::priority_high);
    tbb::parallel_for( 0, 16, [] (int) {
      // do high priority work
      doWork(0.01);
      std::cout << "High\n";
    }, tbb::simple_partitioner(), tcg );
  });
 
  std::thread t1( []() {
    tbb::parallel_for( 0, 16, [] (int) {
      // do normal priority work
      doWork(0.01);
      std::cout << "Normal\n";
    }, tbb::simple_partitioner());
  }); 

  t0.join();
  t1.join();
}

int main() {
  fig_14_03();
  return 0;
}

