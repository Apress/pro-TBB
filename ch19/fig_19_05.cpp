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

#include <iostream>

int main() {

  const tbb::flow::opencl_device_list& devices =
  // if in flow_graph_opencl_node.h we add this line at the end:
  // using interface10::opencl_info::available_devices;
  //tbb::flow::available_devices();
  //else:
           tbb::flow::interface10::opencl_info::available_devices();
  tbb::flow::opencl_device d = *devices.cbegin();
  std::cout << "Platform: "         << d.platform_name()    << '\n';
  std::cout << "Platform profile: " << d.platform_profile() << '\n';
  std::cout << "Platform version: " << d.platform_version() << '\n';
  std::cout << "Platform vendor: "  << d.platform_vendor()  << '\n';
  // std::cout << "Platform extensions: " << d.platform_extensions() << '\n';
  std::cout << '\n';

  for (auto d : devices) {
    std::cout << "Device: "           << d.name() << '\n';
    std::cout << "  Major version: "  << d.major_version();
    std::cout << ";  Minor version: " << d.minor_version() << '\n';
    std::cout << "  Device type: ";
    switch (d.type()) {
      case CL_DEVICE_TYPE_GPU:
        std::cout << "GPU\n";
        break;
      case CL_DEVICE_TYPE_CPU:
        std::cout << "CPU\n";
        break;
      default:
        std::cout << "Unknown\n";
    }
  }

// Find a GPU device:
  tbb::flow::opencl_device_list::const_iterator it = std::find_if(
    devices.cbegin(), devices.cend(),
    [](const tbb::flow::opencl_device& d){
      cl_device_type type;
      d.info(CL_DEVICE_TYPE, type);
      return CL_DEVICE_TYPE_GPU == type;
    });

// ... and print the OpenCL version of the device found
  if (it != devices.cend()){
    tbb::flow::opencl_device od=*it;
    std::cout << od.version() << '\n';
  }
  return 0;
}
