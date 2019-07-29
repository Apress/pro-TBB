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
#include <math.h>
#include <tbb/tbb.h>

double serialPiExample(int num_intervals) {
  double dx = 1.0 / num_intervals;
  double sum = 0.0;
  for (int i = 0; i < num_intervals; ++i) {
    double x = (i+0.5)*dx;
    double h = sqrt(1-x*x);
    sum += h*dx;
  }
  return 4 * sum;
}

template< typename Partitioner >
double reducePiExample(int num_intervals, int grainsize) {
  double dx = 1.0 / num_intervals;
  double sum = tbb::parallel_reduce(
    /* range = */ tbb::blocked_range<int>(0, num_intervals, grainsize), 
    /* idenity = */ 0.0,
    /* func */ 
    [=](const tbb::blocked_range<int>& r, double init) -> double {
      for (int i = r.begin(); i != r.end(); ++i) {
        double x = (i + 0.5)*dx;
        double h = sqrt(1 - x*x);
        init += h*dx;
      }
      return init;
    },
    /* reduction */
    [](double x, double y) -> double {
        return x + y;
    }, 
    /* partitioner */ Partitioner()
  );
  return 4 * sum;
}

template< typename Partitioner >
double deterministicReducePiExample(int num_intervals, int grainsize) {
  double dx = 1.0 / num_intervals;
  double sum = tbb::parallel_deterministic_reduce(
    /* range = */ tbb::blocked_range<int>(0, num_intervals, grainsize), 
    /* identity = */ 0.0,
    /* func */ 
    [=](const tbb::blocked_range<int>& r, double init) -> double {
      for (int i = r.begin(); i != r.end(); ++i) {
        double x = (i + 0.5)*dx;
        double h = sqrt(1 - x*x);
        init += h*dx;
      }
      return init;
    },
    /* reduction */
    [](double x, double y) -> double {
      return x + y;
    },
    /* partitioner */ Partitioner()
  );
  return 4 * sum;
}

static void warmupTBB() {
  tbb::parallel_for(0, tbb::task_scheduler_init::default_num_threads(), [](int) {
    tbb::tick_count t0 = tbb::tick_count::now();
    while ((tbb::tick_count::now() - t0).seconds() < 0.01);
  });
}

int main() {
  const int P = tbb::task_scheduler_init::default_num_threads();
  int num_intervals = 1<<26;
  tbb::tick_count ts_0 = tbb::tick_count::now();
  double spi = serialPiExample(num_intervals);
  tbb::tick_count ts_1 = tbb::tick_count::now();
  double serial_time = (ts_1 - ts_0).seconds();
  std::cout << "serial, " << spi << ", " << serial_time << std::endl;
  warmupTBB();
  std::cout << "speedups relative to serial:" << std::endl;
  std::cout << "gs, r-simple, d-simple, r-static, d-static, r-auto" << std::endl;
  for (int gs = 1; gs <= num_intervals; gs *= 2) {
    reducePiExample<tbb::auto_partitioner>(num_intervals, gs);
    tbb::tick_count t0 = tbb::tick_count::now();
    double v0 = reducePiExample<tbb::auto_partitioner>(num_intervals, gs);
    tbb::tick_count t1 = tbb::tick_count::now();
    double v1 = reducePiExample<tbb::simple_partitioner>(num_intervals, gs);
    tbb::tick_count t2 = tbb::tick_count::now();
    double v2 = reducePiExample<tbb::static_partitioner>(num_intervals, gs);
    tbb::tick_count t3 = tbb::tick_count::now();
    double v3 = deterministicReducePiExample<tbb::simple_partitioner>(num_intervals, gs);
    tbb::tick_count t4 = tbb::tick_count::now();
    double v4 = deterministicReducePiExample<tbb::static_partitioner>(num_intervals, gs);
    tbb::tick_count t5 = tbb::tick_count::now();
    std::cout << gs 
              << ", " << serial_time / (t2-t1).seconds()
              << ", " << serial_time / (t4-t3).seconds()
              << ", " << serial_time / (t3-t2).seconds()
              << ", " << serial_time / (t5-t4).seconds() 
              << ", " << serial_time / (t1-t0).seconds()
              << std::endl;
  }
  return 0;
}

