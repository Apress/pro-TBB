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

void fig_17_30() {
  tbb::task_arena a2{2}; 
  tbb::task_group_context tcg2;
  tcg2.set_priority(tbb::priority_normal);
  tbb::flow::graph g2{tcg2};
  a2.execute([&]() {
    g2.reset();
  });

  tbb::task_arena a4{4}; 
  tbb::task_group_context tcg4;
  tcg4.set_priority(tbb::priority_high);
  tbb::flow::graph g4{tcg4};
  a4.execute([&]() {
    g4.reset();
  });

  tbb::flow::function_node<int, int> 
  g2_node{g2, tbb::flow::serial, WhereAmIRunningBody("g2_node")};

  tbb::flow::function_node<int, int> 
  g4_node{g4, tbb::flow::serial, WhereAmIRunningBody("g4_node")};

  g2_node.try_put(0);
  g2.wait_for_all();

  g4_node.try_put(1);
  g4.wait_for_all();

  tbb::flow::make_edge(g2_node,g4_node);
  g2_node.try_put(2);
  g2.wait_for_all();
  g4.wait_for_all();
}

int main(int argc, char *argv[]) {
  warmupTBB();
  fig_17_30();
  return 0;
}

