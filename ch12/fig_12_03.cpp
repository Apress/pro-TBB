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

void doWork();

thread_local int local_i = -1;

void fig_12_3() {
  const int P = tbb::task_scheduler_init::default_num_threads();
  
  tbb::parallel_for(0, P,
    [](int i) {
      const int N = 1000;
      local_i = i;
      tbb::parallel_for(0, N, [](int j) {doWork();});
      if (local_i != i) {
        std::cout << "Unexpected local_i!!! ";
      }
    }
  );
  std::cout << std::endl;
}

void doWork() {
  tbb::tick_count t0 = tbb::tick_count::now();
  while ((tbb::tick_count::now() - t0).seconds() < 0.001);
}

int main() {
  std::cout << "WARNING: This example demonstrates a possibe behavior!" << std::endl
            << "WARNING: The bad behavior is not guaranteed for each run."
            << std::endl << std::flush;
  fig_12_3();
  std::cout << "Done." << std::endl;
  return 0;
}

