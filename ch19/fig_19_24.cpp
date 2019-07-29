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
#include <numeric>
#include <vector>
#include <tbb/flow_graph.h>
#include <tbb/flow_graph_opencl_node.h>
#include <tbb/tick_count.h>
#include <tbb/atomic.h>

int main(int argc, const char* argv[]) {

  size_t vsize = 4;

  tbb::flow::graph g;

  using buffer_f = tbb::flow::opencl_buffer<cl_int>;
  buffer_f Adevice{vsize};
  int* Ahost = Adevice.data();
  std::iota(Ahost, Ahost+vsize, 0); // 0, 1, 2, 3, ...

  //GPU node:
  tbb::flow::opencl_program<> program{std::string{"fig_19_24.cl"}};
  tbb::atomic<int> device_num{0};
  tbb::flow::opencl_node<std::tuple<buffer_f>> gpu_node{g,
    program.get_kernel("cl_inc"),
    [&device_num](auto& f){
      auto d = *(f.devices().begin() + device_num++ % f.devices().size());
      std::cout << "Running on "<< d.name() << '\n';
      return d;
    }};
  gpu_node.set_range({{static_cast<int>(vsize)}});
  gpu_node.set_args(tbb::flow::port_ref<0>);
  for(int i=0; i<3; i++){
    std::cout << "Iteration: " << i << '\n';
    tbb::flow::input_port<0>(gpu_node).try_put(Adevice);
  }

  g.wait_for_all();

  std::vector<int> AGold(vsize);
  std::iota(AGold.begin(), AGold.end(), 3); // 3, 4, 5, 6, ...
  if (! std::equal(Adevice.begin(), Adevice.end(), AGold.begin()))
    std::cout << "Errors in the heterogeneous computation.\n";

#ifdef VERBOSE
  std::cout << "Results: " << '\n';
  for (size_t i = 0; i < vsize; i++) {
    std::cout << Ahost[i] << ", ";
  }
  std::cout<< '\n';
#endif
  return 0;
}
