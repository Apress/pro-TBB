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

#define TBB_PREVIEW_FLOW_GRAPH_NODES 1
#define TBB_PREVIEW_FLOW_GRAPH_FEATURES 1

#include <tbb/flow_graph_opencl_node.h>

#include <cmath>
#include <stdexcept>
#include <string>

int main() {
  using buffer_i = tbb::flow::opencl_buffer<int>;
  try {
    constexpr size_t n = 10;

    tbb::flow::graph g;

    tbb::flow::function_node<int, buffer_i> filler0{g,
      tbb::flow::unlimited,
      [n](int i){
        buffer_i b{n};
        std::fill(b.begin(), b.end(), i);
        return b;
      }};

    tbb::flow::function_node<int, buffer_i> filler1 = filler0;

    tbb::flow::opencl_program<> program{std::string{"mul.cl"}};
    tbb::flow::opencl_node<std::tuple<buffer_i, buffer_i>> gpu_node{g,
                                         program.get_kernel("mul")};
    gpu_node.set_range({{ n }});

    tbb::flow::function_node<buffer_i> checker{g,
      tbb::flow::serial,
      [](const buffer_i& b){
        for (int v : b){
          int r = static_cast<int>(std::sqrt(v) + .5);
          if (r*r != v)
          throw std::runtime_error(std::to_string(v) +
                         " is not a square of any integer number" );
        }
      }};

    tbb::flow::make_edge(filler0, tbb::flow::input_port<0>(gpu_node));
    tbb::flow::make_edge(filler1, tbb::flow::input_port<1>(gpu_node));
    tbb::flow::make_edge(tbb::flow::output_port<0>(gpu_node), checker);

    for (int i = 1; i<=1000; ++i){
      filler0.try_put(i);
      filler1.try_put(i);
    }
    g.wait_for_all();
  }
  catch (std::exception& e){
    std::cerr << "Liar!!: " << e.what() << std::endl;
    return 0;
  }
  std::cout << "Test passed!" << '\n';
  return 0;
}
