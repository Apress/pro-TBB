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

void fig_17_19() {
  tbb::task_scheduler_init init{8};
  tbb::task_arena a2{2};
  tbb::task_arena a4{4};
  
  tbb::flow::graph g;
  tbb::flow::function_node<std::string> f{g, tbb::flow::unlimited,
    [](const std::string& str) {
      int P = tbb::this_task_arena::max_concurrency();
      std::cout << str << " : " << P << std::endl;
    }
  };

  std::cout << "Without reset:" << std::endl;

  f.try_put("default");
  g.wait_for_all();
  a2.execute( [&]() { 
    f.try_put("a2"); 
    g.wait_for_all();
  } );
  a4.execute( [&]() { 
    f.try_put("a4"); 
    g.wait_for_all();
  } );

  std::cout << "With reset:" << std::endl;

  f.try_put("default");
  g.wait_for_all();
  a2.execute( [&]() { 
    g.reset();
    f.try_put("a2"); 
    g.wait_for_all();
  } );
  a4.execute( [&]() { 
    g.reset();
    f.try_put("a4"); 
    g.wait_for_all();
  } );
}

int main() {
  fig_17_19();
  return 0;
}

