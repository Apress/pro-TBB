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

//#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION 120
#define TBB_PREVIEW_GLOBAL_CONTROL 1

#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <math.h>
#include <tbb/flow_graph.h>
#include <tbb/tick_count.h>
#include <tbb/compat/thread>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/task_scheduler_init.h>
#include <tbb/global_control.h>
#include "CL/cl2.hpp"

int vsize;
float* Ahost;                       // Host view of A, B and C arrays
float* Bhost;                       //
float* Chost;                       //
cl::Buffer Adevice;                 // Device view of A, B and C arrays
cl::Buffer Bdevice;                 //
cl::Buffer Cdevice;                 //
cl::CommandQueue queue;             // OpenCL command queue
cl::Program program;                // OpenCL program

std::string read_kernel_source(const std::string& path){
  std::ifstream file_stream{path};
  if (file_stream.is_open()){
    std::ostringstream kernel_code;
    kernel_code << file_stream.rdbuf();
    file_stream.close();
    return kernel_code.str();
  }
  return "";
}

void opencl_initialize(){
//  try {
    // Find platforms
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    if(platforms.size()==0){
      std::cout << "Oops, no platform found!\n";
      exit(1);
    }
    std::cout << "Number of platforms: " << platforms.size() << "\n";
    // Find first GPU device
    std::vector<cl::Device> devices;
    bool found = false;
    for(auto& platform : platforms){
      std::cout << "Platform name: " << platform.getInfo<CL_PLATFORM_NAME>() << '\n';
      if(platform.getDevices(CL_DEVICE_TYPE_GPU, &devices)==CL_SUCCESS){
        found = true;
        break;
      }
      else std::cout << "No GPU found in this platform \n";
    }
    if(!found){
      std::cout << "Oops, no GPU device found!\n";
      exit(1);
    }
    // Choose first GPU device:
    cl::Device device=devices[0];
    std::cout << "Device name: "<< device.getInfo<CL_DEVICE_NAME>();
    std::cout << " with " << device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>() << " compute units" << '\n';

    // Create the context
    cl::Context context{device};

    // Create a command queue
    queue = cl::CommandQueue{context, device};

    // Create the program from the kernel file
    std::string kernel_source = read_kernel_source("triad.cl");
    cl_int err = CL_SUCCESS;
    program = cl::Program{context, kernel_source, true, &err};
    if(err!=CL_SUCCESS){
      std::cout << "Error building: "
                << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << '\n';
      exit(1);
    }
    // Create  input and output device arrays
    Adevice = cl::Buffer{context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, sizeof(float)*vsize};
    Bdevice = cl::Buffer{context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, sizeof(float)*vsize};
    Cdevice = cl::Buffer{context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, sizeof(float)*vsize};
    Ahost=(float*)queue.enqueueMapBuffer(Adevice, CL_TRUE, CL_MAP_WRITE | CL_MAP_READ, 0, sizeof(float) * vsize, NULL, NULL, &err);
    Bhost=(float*)queue.enqueueMapBuffer(Bdevice, CL_TRUE, CL_MAP_WRITE | CL_MAP_READ, 0, sizeof(float) * vsize, NULL, NULL, &err);
    Chost=(float*)queue.enqueueMapBuffer(Cdevice, CL_TRUE, CL_MAP_WRITE | CL_MAP_READ, 0, sizeof(float) * vsize, NULL, NULL, &err);
    for(int i = 0; i < vsize; i++){
      Ahost[i] = i;
      Bhost[i] = i;
    }
    err = queue.enqueueUnmapMemObject(Adevice, Ahost, NULL, NULL);
    err|= queue.enqueueUnmapMemObject(Bdevice, Bhost, NULL, NULL);
    if (err != CL_SUCCESS)
    {
      std::cout << "Error: Failed to unmap Ahost or Bhost!\n";
      exit(1);
    }
 // }
  //catch(cl::Error& err) {
   // std::cerr << "ERROR: " << err.what()
    //          << "(" << err.err() << ")" << '\n';
   // std::cerr << "Exiting...\n";
    //exit(-1);
 // }
}

using async_node_type = tbb::flow::async_node< float, double>;
using gateway_type = async_node_type::gateway_type;

class AsyncActivity {
  tbb::task_arena a;
public:
  AsyncActivity() {
    a = tbb::task_arena{1,0};
  }
  using async_node_t = tbb::flow::async_node<float, double>;
  using gateway_t = async_node_t::gateway_type;
  void run(float offload_ratio, gateway_t& gateway){
    gateway.reserve_wait();
    a.enqueue([&,offload_ratio]()
      {
        auto t = tbb::tick_count::now();
        // Make triad kernel, NDRange and launch
        auto triad_kernel =
        cl::KernelFunctor<cl::Buffer&, cl::Buffer&, cl::Buffer&>{program, "triad"};
        cl::EnqueueArgs q_args{
          queue,
          cl::NDRange{static_cast<size_t>(ceil(vsize*offload_ratio))}
        };
        triad_kernel(q_args, Adevice, Bdevice, Cdevice).wait();
        //queue.finish();
        // old cl.hpp-compliant style:
        // cl::compatibility::make_kernel<cl::Buffer&,cl::Buffer&,cl::Buffer&>
        // triad{cl::Kernel{program, "triad"}};
        // triad(q_args, Adevice, Bdevice, Cdevice).wait();
        gateway.try_put((tbb::tick_count::now()-t).seconds());
        gateway.release_wait();
      });
    }
  };

int main(int argc, const char* argv[]) {
  int nth = 4;
  vsize = 100000000;
  float ratio = 0.5;
  float alpha = 0.5;
  opencl_initialize(); // OpenCL boilerplate

  tbb::task_scheduler_init init{nth};
  auto mp=tbb::global_control::max_allowed_parallelism;
  tbb::global_control gc(mp, nth+1); //One more thread, but sleeping
  tbb::flow::graph g;

  bool n = false;
  tbb::flow::source_node<float> in_node{g, [&](float& offload_ratio) {
    if(n) return false;
    offload_ratio = ratio;
    n = true;
    return true;
  },false};

  tbb::flow::function_node<float, double> cpu_node{g,
    tbb::flow::unlimited,
    [&](float offload_ratio) -> double {
      auto t=tbb::tick_count::now();
      tbb::parallel_for(
        tbb::blocked_range<size_t>{
          static_cast<size_t>(ceil(vsize*offload_ratio)),
          static_cast<size_t>(vsize)
        },
        [&](const tbb::blocked_range<size_t>& r){
          for (size_t i = r.begin(); i < r.end(); ++i)
            Chost[i] = Ahost[i] + alpha * Bhost[i];
        }
      );
      return (tbb::tick_count::now() - t).seconds();
  }};
// … to be continued …
  using async_node_t = tbb::flow::async_node<float, double>;
  using gateway_t = async_node_t::gateway_type;
  AsyncActivity asyncAct;
  async_node_t a_node {g, tbb::flow::unlimited,
    [&asyncAct](const float& offload_ratio, gateway_t& gateway) {
      asyncAct.run(offload_ratio, gateway);
    }
  };

  using join_t = tbb::flow::join_node <std::tuple<double,double>,
                                       tbb::flow::queueing>;
  join_t node_join{g};

  tbb::flow::function_node<join_t::output_type> out_node{g,
    tbb::flow::unlimited,
    [&](const join_t::output_type& times){
      // Serial execution
      //std::unique_ptr<float[]> CGold{new float[vsize]};
      std::vector<float>CGold(vsize);
      //if your compiler supports c++14 you can get rid of the "new"
      //std::unique_ptr<float[]> CGold = std::make_unique<float[]>(vsize);
      auto t = tbb::tick_count::now();
      std::transform(Ahost, Ahost + vsize, Bhost, CGold.begin(),
                     [&](float a, float b)->float{return a+alpha*b;});
      double ts = (tbb::tick_count::now() - t).seconds();

  #ifdef VERBOSE
      std::cout << "Results: " << '\n';
      for (int i = 0; i < vsize; i++) {
        std::cout << Chost[i] << ", ";
      }
      std::cout<< '\n';
  #endif
      // Compare golden triad with heterogeneous triad
      if (!std::equal(Chost, Chost+vsize, CGold.begin()))
        std::cout << "Error!!\n";
      else
        std::cout << "Heterogenous computation correct.\n";

      std::cout << "Time serial cpu: " << ts << " seconds" << '\n';
      std::cout << "Time cpu: " << std::get<1>(times) << " seconds" << '\n';
      std::cout << "Time gpu: " << std::get<0>(times) << " seconds" << '\n';
  }};
// … to be continued …
  tbb::flow::make_edge(in_node, a_node);
  tbb::flow::make_edge(in_node, cpu_node);
  tbb::flow::make_edge(a_node, tbb::flow::input_port<0>(node_join));
  tbb::flow::make_edge(cpu_node, tbb::flow::input_port<1>(node_join));
  tbb::flow::make_edge(node_join, out_node);

  auto gt = tbb::tick_count::now();
  in_node.activate();
  g.wait_for_all();

  std::cout << "Total graph time: " << (tbb::tick_count::now() - gt).seconds() << " seconds" <<'\n';
  return 0;
} // End of main()!

/*Output:
./triad 4 100000000 0.1
Platform name: Apple
Device name: Intel(R) HD Graphics 530 with 24 compute units
Heterogenous computation correct.
Time serial cpu: 0.309079 seconds
Time cpu: 0.132203 seconds
Time gpu: 0.130705 seconds
Total graph time: 0.704758 seconds
*/
