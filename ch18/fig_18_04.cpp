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
#include <tbb/flow_graph.h>
#include <tbb/tick_count.h>
#include <tbb/compat/thread>

void async_world() {
  tbb::flow::graph g;
  bool n = false;

  //Source node:
  tbb::flow::source_node<int> in_node{g,
    [&](int& a) {
      if (n) return false;
      std::cout << "Async ";
      a = 10;
      n = true;
      return true;
    },
    false
  };

  //Danger! Don't do this!
  //Function node (previously implemented as async_node
  tbb::flow::function_node<int, int> a_node{g, tbb::flow::unlimited,
    [&](const int& input) -> int {
      int output;
      std::thread asyncThread{[&,input]{
        std::cout << "World! Input: "<< input << '\n';
        output = input + 1;
      }};
      asyncThread.join(); // a worker thread blocks here!
      return output;
    }};

  //Output node:
  tbb::flow::function_node<int> out_node{g, tbb::flow::unlimited,
    [](int const& a_num){
      std::cout << "Bye! Received: "<< a_num<< '\n';
    }
  };

  //Edges:
  make_edge(in_node, a_node);
  make_edge(a_node, out_node);

  //Run!
  in_node.activate();
  g.wait_for_all();
}

int main() {
  tbb::tick_count mainStartTime = tbb::tick_count::now();
  async_world();
  auto time = (tbb::tick_count::now() - mainStartTime).seconds();
  std::cout << "Execution time = " << time << " seconds."<< '\n';
  return 0;
}
