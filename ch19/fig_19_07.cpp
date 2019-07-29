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
#include <tbb/flow_graph_opencl_node.h>
#include <tbb/tick_count.h>

class gpu_device_selector{
public:
  template<typename DeviceFilter>
    tbb::flow::opencl_device operator()(tbb::flow::opencl_factory<DeviceFilter>& f) {
    auto it = std::find_if(
      f.devices().cbegin(), f.devices().cend(),
      [](const tbb::flow::opencl_device& d) {
        if (d.type() == CL_DEVICE_TYPE_GPU){
          std::cout << "Found GPU!" << '\n';
          return true;
        }
        return false;
      });
    if (it == f.devices().cend()){
      std::cout << "No GPU found!." << '\n';
      return *f.devices().cbegin(); //Return the first one
    }
    std::cout << "Running on "<< it->name() << '\n';
    return *it;
  }
};

class gpu_device_selector_short{
public:
  template<typename DeviceFilter>
    tbb::flow::opencl_device operator()(tbb::flow::opencl_factory<DeviceFilter>& f) {
      //tbb::flow::opencl_device_list::const_iterator it = std::find_if(
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

  tbb::flow::graph g;
  //Source node:
  bool n = false;
  using buffer_t = tbb::flow::opencl_buffer<cl_char>;
  tbb::flow::source_node<buffer_t> in_node{g, [&](buffer_t& a){
    if(n) return false;
    else {
      std::cout << "Hello ";
      char str[] = "OpenCL_Node\n";
      a = buffer_t{sizeof(str)};
      std::copy_n(str, sizeof(str), a.begin());
      n = true;
      return true;
    }
  },false};

  //GPU node:
  gpu_device_selector gpu_selector;
  //gpu_device_selector_short gpu_selector; //alternative
  tbb::flow::opencl_program<> program{std::string{"hello.cl"}};
  tbb::flow::opencl_node<std::tuple<buffer_t>> gpu_node{g,
                              program.get_kernel("cl_print"),
                              gpu_selector};
  gpu_node.set_range({{1}});

  //Output node:
  tbb::flow::function_node<buffer_t> out_node{g, tbb::flow::unlimited,
    [](buffer_t const& m){
      char *str = (char*) m.begin();
      std::cout << "Bye! Received from: " << str;
    }
  };

  //Make edges and Run!
  tbb::flow::make_edge(in_node, gpu_node);
  tbb::flow::make_edge(gpu_node, out_node);
  in_node.activate();
  g.wait_for_all();

  return 0;
}
