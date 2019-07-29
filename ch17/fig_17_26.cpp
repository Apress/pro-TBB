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
#include <string>
#include <tbb/tbb.h>

static inline void spinWaitForAtLeast(double sec) {
  tbb::tick_count t0 = tbb::tick_count::now();
  while ((tbb::tick_count::now() - t0).seconds() < sec);
}

void warmupTBB() {
  tbb::parallel_for(0, tbb::task_scheduler_init::default_num_threads(), 
  [](int) {
    spinWaitForAtLeast(0.001);
  });
}

void fig_17_26() {
  tbb::flow::graph g;

  tbb::flow::function_node<int, int> node1(g, tbb::flow::serial,
    [](int i) { return i; }
  );

  tbb::flow::function_node<int, int> node2(g, tbb::flow::serial,
    [](int i) { 
      throw i;
      return i; 
    }
  );

  tbb::flow::function_node<int, int> node3(g, tbb::flow::serial,
    [](int i) { return i; }
  );

  tbb::flow::make_edge(node1,node2);
  tbb::flow::make_edge(node2,node3);
  node1.try_put(1);
  node2.try_put(2);
  g.wait_for_all();
}

int main(int argc, char *argv[]) {
  warmupTBB();
  std::cout << "NOTE: this example intentionally fails!" << std::endl;
  fig_17_26();
  return 0;
}

