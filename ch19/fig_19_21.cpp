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

#include <algorithm>
#include <random>
#include <cstdio>
#include <tbb/flow_graph.h>
#include <tbb/flow_graph_opencl_node.h>
#include <tbb/tick_count.h>

class gpu_device_selector{
public:
  template<typename DeviceFilter>
    tbb::flow::opencl_device operator()(tbb::flow::opencl_factory<DeviceFilter>& f) {
    auto it = std::find_if(f.devices().cbegin(), f.devices().cend(),
      [](const tbb::flow::opencl_device& d) {
        return  d.type() == CL_DEVICE_TYPE_GPU;
      });
    if (it == f.devices().cend())
      return *f.devices().cbegin(); //Return the first one
    return *it;
  }
};

int main(int argc, const char* argv[]) {

  int h = 4;
  int w = 4;
  size_t vsize = h * w;

  tbb::flow::graph g;

  using buffer_f = tbb::flow::opencl_buffer<cl_float>;
  buffer_f Adevice{vsize};
  buffer_f Bdevice{vsize};
  buffer_f Cdevice{vsize};
  float* Ahost = Adevice.data();
  float* Bhost = Bdevice.data();

  // Initialize A and B
  std::iota(Ahost, Ahost+vsize, 0); // 0, 1, 2, 3, ...
  std::iota(Bhost, Bhost+vsize, 0); // 0, 1, 2, 3, ...

  //GPU node:
  // Try a different device if the used one does not printf
  gpu_device_selector s;
  tbb::flow::opencl_program<> program{std::string{"fig_19_19.cl"}};
  // //Option 1:
  tbb::flow::opencl_node<std::tuple<buffer_f>> gpu_node{g,
                      program.get_kernel("cl_print"),
                      s};
  gpu_node.set_range({{w, h}, {w/2, h/2}});
  gpu_node.set_args(Adevice, Bdevice, tbb::flow::port_ref<0>, w);
  tbb::flow::input_port<0>(gpu_node).try_put(Cdevice);

  //Option 2:
  // tbb::flow::opencl_node<std::tuple<buffer_f, tbb::flow::opencl_range>>
  //             gpu_node{g, program.get_kernel("cl_print"), s};
  // gpu_node.set_range(tbb::flow::port_ref<1>);
  // gpu_node.set_args(Adevice, Bdevice, tbb::flow::port_ref<0>, w);
  // tbb::flow::input_port<0>(gpu_node).try_put(Cdevice);
  // tbb::flow::input_port<1>(gpu_node).try_put(
  //                     tbb::flow::opencl_range({{w, h}, {w/2, h/2}}));

  g.wait_for_all();

  float* Chost = Cdevice.data();
  //std::unique_ptr<float[]> CGold{new float[vsize]};
  std::vector<float> CGold(vsize);
  // std::transform(Ahost, Ahost + vsize, Bhost, &CGold[0],
  //                std::plus<float>());
  // std::transform(Ahost, Ahost + vsize, Bhost, CGold.get(),
  //                std::plus<float>());
  std::transform(Ahost, Ahost + vsize, Bhost, CGold.begin(),
                  std::plus<float>());

  #ifdef VERBOSE
      std::cout << "Results: " << '\n';
      for (size_t i = 0; i < vsize; i++) {
        std::cout << Chost[i] << ", ";
      }
      std::cout<< '\n';
  #endif
  // Check correctness:
  if (! std::equal (Chost, Chost+vsize, CGold.begin()))
    std::cout << "Errors in the GPU computation.\n";

  return 0;
}
