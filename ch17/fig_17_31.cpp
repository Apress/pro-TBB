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

struct WhereAmIRunningBody {
  std::string node_name;
  WhereAmIRunningBody(const char *name) : node_name(name) {}

  int operator()(int i) {
    int P = tbb::this_task_arena::max_concurrency();
    std::string priority = "normal";
    if (tbb::task::self().group()->priority() == tbb::priority_high ) {
      priority = "high";
    }
    std::cout << i << ":" << node_name 
              << " executing in arena " << P 
              << " with priority " << priority << std::endl; 
    spinWaitForAtLeast(0.1);
    return i;
  }
};

void fig_17_31() {
  tbb::flow::graph g1;
  tbb::flow::function_node<int, int> 
  g1_node1{g1, tbb::flow::serial,
    [](int i) {
      std::cout << "g1_node1\n";
      spinWaitForAtLeast(i*0.1);
      return i;
    }
  };
  tbb::flow::function_node<int, int> 
  g1_node2{ g1, tbb::flow::serial,
    [](int i) {
      std::cout << "g1_node2\n";
      spinWaitForAtLeast(i*0.1);
      return i;
    }
  };
  tbb::flow::make_edge(g1_node1,g1_node2);

  tbb::flow::graph g2;
  tbb::flow::function_node<int, int> 
  g2_node1{g2, tbb::flow::serial,
    [](int i) {
      std::cout << "g2_node1\n";
      spinWaitForAtLeast(i*0.1);
      return i;
    }
  };
  tbb::flow::function_node<int, int> 
  g2_node2{g2, tbb::flow::serial,
    [&g1_node1](int i) {
      std::cout << "g2_node2\n";
      spinWaitForAtLeast(i*0.1);
      g1_node1.try_put(i);
      return i;
    }
  };
  tbb::flow::make_edge(g2_node1,g2_node2);

  g2_node1.try_put(1);
  g1.wait_for_all(); // returns immediately
  g2.wait_for_all(); // returns after g2_node1 and g2_node2

  std::cout << "At the end of the function\n";
  // we reach here before g1 (started by g2) is done
}

int main(int argc, char *argv[]) {
  std::cout << "Note: This example demonstrates a failure and may crash!" << std::endl;
  warmupTBB();
  fig_17_31();
  return 0;
}

