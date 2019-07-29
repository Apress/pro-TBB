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

const int M = 2*tbb::task_scheduler_init::default_num_threads();

void splitRunAndWait() {
  tbb::task_group tg;

  for ( int i=0; i<M; ++i ) {
    tbb::this_task_arena::isolate( [&tg]{
      // Run in inner region
      tg.run( []{
        const int N = 10000;
        tbb::parallel_for(0,N,[](int i) {
          doWork();
        });
      });
    });
  }
  tg.wait();
}

void fig_12_6() {
  tbb::parallel_for( 0, M, []( int ) {
    tbb::this_task_arena::isolate( [] {
      splitRunAndWait();
    } );
  } );
}

void doWork() {
  tbb::tick_count t0 = tbb::tick_count::now();
  while ((tbb::tick_count::now() - t0).seconds() < 1e-6);
}

int main() {
  std::cout << "WARNING: This example demonstrates a deadlock!" 
            << std::endl << std::flush;
  fig_12_6();
  std::cout << "Done." << std::endl;
  return 0;
}

