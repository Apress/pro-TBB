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

void fig_17_23() {
  int P = tbb::task_scheduler_init::default_num_threads();
  tbb::concurrent_vector<std::string> trace;
  double spin_time = 1e-3;
  tbb::flow::graph g;

  int src_cnt = 0;
  tbb::flow::source_node<int> source{g,
    [&src_cnt, P, spin_time](int& i) -> bool {
       if (src_cnt < P) {
         i = src_cnt++;
         spinWaitForAtLeast(spin_time);
         return true;
       }
       return false;
    }, false
  };

  tbb::flow::function_node<int> unlimited_node(g, tbb::flow::unlimited,
    [&trace, P, spin_time](int i) {
      int tid = tbb::this_task_arena::current_thread_index();
      trace.push_back(std::to_string(i) + " started by " 
                      + std::to_string(tid));
      tbb::parallel_for(0, P-1, 
        [spin_time](int i) {
          spinWaitForAtLeast((i+1)*spin_time);
        } 
      );
      trace.push_back(std::to_string(i) + " completed by " 
                      + std::to_string(tid));
    }
  );

  tbb::flow::make_edge(source, unlimited_node);
#if TBB_PREVIEW_FLOW_GRAPH_TRACE
  s.set_name("source");
  n.set_name("unlimited_node");
#endif
  source.activate();
  g.wait_for_all();

  for (auto s : trace) {
    std::cout << s << std::endl;
  }
}

void noMoonlighting() {
  int P = tbb::task_scheduler_init::default_num_threads();
  tbb::concurrent_vector<std::string> trace;
  double spin_time = 1e-3;
  tbb::flow::graph g;

  int src_cnt = 0;
  tbb::flow::source_node<int> source{g,
    [&src_cnt, P, spin_time](int& i) -> bool {
       if (src_cnt < P) {
         i = src_cnt++;
         spinWaitForAtLeast(spin_time);
         return true;
       }
       return false;
    }, false
  };

  tbb::flow::function_node<int> unlimited_node{g, tbb::flow::unlimited,
    [&trace, P,spin_time](int i) {
      int tid = tbb::this_task_arena::current_thread_index();
      trace.push_back(std::to_string(i) + " started by " 
                      + std::to_string(tid));
      tbb::this_task_arena::isolate([P,spin_time]() {
        tbb::parallel_for(0, P-1, 
          [spin_time](int i) {
            spinWaitForAtLeast((i+1)*spin_time);
          } 
        );
      });
      trace.push_back(std::to_string(i) + " completed by " 
                      + std::to_string(tid));
    }
  };

  tbb::flow::make_edge(source, unlimited_node);
#if TBB_PREVIEW_FLOW_GRAPH_TRACE
  s.set_name("s");
  n.set_name("n");
#endif
  source.activate();
  g.wait_for_all();

  for (auto s : trace) {
    std::cout << s << std::endl;
  }
}

int main() {
  warmupTBB();
  std::cout << "Without isolation:" << std::endl;
  fig_17_23();
  spinWaitForAtLeast(10e-3); 

  warmupTBB();
  std::cout << std::endl << "With isolation:" << std::endl;
  noMoonlighting();
  return 0;
}

