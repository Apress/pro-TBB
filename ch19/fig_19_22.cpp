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

#include <cstdio>
#include <random>
#include <tbb/flow_graph.h>
#include <tbb/flow_graph_opencl_node.h>
#include <tbb/tick_count.h>

int main(int argc, const char* argv[]) {

  int rows = 4;
  int cols = 4;
  size_t vsize = rows * cols;

  tbb::flow::graph g;

  using buffer_f = tbb::flow::opencl_buffer<cl_float>;
  buffer_f Adevice{vsize};
  buffer_f Bdevice{vsize};
  buffer_f Cdevice{vsize};
  float* Ahost = Adevice.data();
  float* Bhost = Bdevice.data();

  // Initialize random number generator
  std::random_device seed;    // Random device seed
  std::mt19937 mte{seed()};   // mersenne_twister_engine
  std::uniform_int_distribution<> uniform{0, 256};
  // Initialize A and B
  std::generate(Ahost, Ahost+vsize, [&]{return uniform(mte);});
  std::generate(Bhost, Bhost+vsize, [&]{return uniform(mte);});

  //GPU node 1:
  tbb::flow::opencl_program<> program{std::string{"fig_19_23.cl"}};
  using tuple_gpu = std::tuple<buffer_f>;

  tbb::flow::opencl_node<tuple_gpu> gpu_node1{g,
    program.get_kernel("cl_add"),
    [](auto& f){
      auto d = *(f.devices().begin());
      if(f.devices().size() > 1)
        d = *(f.devices().begin() + 1);
      std::cout << "Running gpu_node1 on " << d.name()<< '\n';
      return d;
    }};
  gpu_node1.set_range({{cols, rows}});
  gpu_node1.set_args(Adevice, Bdevice, tbb::flow::port_ref<0>);

  //GPU node 2:
  tbb::flow::opencl_node<tuple_gpu> gpu_node2{g,
    program.get_kernel("cl_sub"),
    [](auto& f){
      auto d = *(f.devices().begin());
      if(f.devices().size() > 2)
        auto d = *(f.devices().begin() + 2);
      std::cout << "Running gpu_node2 on " << d.name()<< '\n';
      return d;
    }};
  gpu_node2.set_range({{cols, rows}});
  gpu_node2.set_args(Adevice, Bdevice, tbb::flow::port_ref<0>);

  //Output node:
  tbb::flow::function_node<buffer_f> out_node{g, tbb::flow::unlimited,
    [&](buffer_f const& Cdevice){
      float* Chost = Cdevice.data();
      if (! std::equal(Chost, Chost+vsize, Ahost))
        std::cout << "Errors in the heterogeneous computation.\n";
    }
  };

  make_edge(tbb::flow::output_port<0>(gpu_node1),
            tbb::flow::input_port<0>(gpu_node2));
  make_edge(tbb::flow::output_port<0>(gpu_node2),
            out_node);
  tbb::flow::input_port<0>(gpu_node1).try_put(Cdevice);
  g.wait_for_all();

  return 0;
}
