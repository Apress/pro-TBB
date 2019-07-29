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
#define CL_HPP_MINIMUM_OPENCL_VERSION 200
#define CL_HPP_TARGET_OPENCL_VERSION 200
#define VERBOSE

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
#include <CL/cl2.hpp>

int vsize;
using svmalloc_t = cl::SVMAllocator<float, cl::SVMTraitFine<cl::SVMTraitReadWrite<>>>;
std::vector<float,svmalloc_t> A;   // A, B and C arrays
std::vector<float,svmalloc_t> B;   //
std::vector<float,svmalloc_t> C;   //
cl::DeviceCommandQueue queue;      // OpenCL command queue
cl::Program program;               // OpenCL program

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
#ifdef CL_HPP_ENABLE_EXCEPTIONS
  try {
#endif
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
    for(auto& plat : platforms){
      std::cout << "Platform name: " << plat.getInfo<CL_PLATFORM_NAME>() << '\n';
      if(plat.getDevices(CL_DEVICE_TYPE_GPU, &devices)==CL_SUCCESS){
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
    cl::Device device = devices[0];
    std::cout << "Device name: " << device.getInfo<CL_DEVICE_NAME>();
    std::cout << " with " << device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>() << " compute units\n";
    std::cout << "OpenCL version: " << device.getInfo<CL_DEVICE_OPENCL_C_VERSION>() << '\n';
    auto svmcapability = device.getInfo<CL_DEVICE_SVM_CAPABILITIES>();
    if (svmcapability & CL_DEVICE_SVM_FINE_GRAIN_BUFFER){
      fprintf(stderr, "SVM FINE GRAIN BUFFER supported!\n");
    }
    else{
      fprintf(stderr, "SVM FINE GRAIN BUFFER not supported! Exit!\n");
      exit(1);
    }
    if (svmcapability & CL_DEVICE_SVM_COARSE_GRAIN_BUFFER)
      fprintf(stderr, "SVM COARSE GRAIN BUFFER supported!\n");

    if (svmcapability & CL_DEVICE_SVM_FINE_GRAIN_SYSTEM)
      fprintf(stderr, "SVM FINE GRAIN SYSTEM supported!\n");

    if (svmcapability & CL_DEVICE_SVM_ATOMICS)
      fprintf(stderr, "SVM ATOMICS supported!\n");

    // Create the context
    cl_int err = CL_SUCCESS;
    cl::Context context = cl::Context{device, NULL, NULL, NULL, &err};
    context = cl::Context::setDefault(context);
    if(err != CL_SUCCESS){
      std::cout << "Error setting default context: " << '\n';
      exit(1);
    }

    // Create a command queue
    queue = cl::DeviceCommandQueue::makeDefault(context, device, &err);

    // Create the program from the kernel file
    std::string kernel_source = read_kernel_source("triad.cl");
    program = cl::Program{context, kernel_source, true, &err};
    if(err!=CL_SUCCESS){
      std::cout << "Error building: "
                << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device)<< '\n';
      exit(1);
    }
    // Allocate SVM arrays
    svmalloc_t svmAlloc;
    A=std::vector<float, svmalloc_t>{static_cast<size_t>(vsize), 0, svmAlloc};
    B=std::vector<float, svmalloc_t>{static_cast<size_t>(vsize), 0, svmAlloc};
    C=std::vector<float, svmalloc_t>{static_cast<size_t>(vsize), 0, svmAlloc};
    // Initialize A and B
    for(int i = 0; i < vsize; i++){
      A[i] = i;
      B[i] = i;
    }
#ifdef CL_HPP_ENABLE_EXCEPTIONS
  }
  catch(cl::Error& err) {
    std::cerr << "ERROR: " << err.what()
              << "(" << err.err() << ")" << '\n';
    std::cerr << "Exiting...\n";
    exit(-1);
  }
#endif
}

using async_node_type = tbb::flow::async_node< float, double>;
using gateway_type = async_node_type::gateway_type;

class AsyncActivity {
public:
  ~AsyncActivity() {
    asyncThread.join();
  }
  using async_node_type = tbb::flow::async_node<float, double>;
  using gateway_type = async_node_type::gateway_type;
  void run(float offload_ratio, gateway_type& gateway){
    gateway.reserve_wait();
    asyncThread=std::thread{[&,offload_ratio]()
      {
        auto t = tbb::tick_count::now();
        // Make triad kernel, NDRange and launch
        cl_int err;
        cl::EnqueueArgs q_args{
          cl::NDRange{static_cast<size_t>(ceil(vsize*offload_ratio))}
        };
        // Old cl.hpp compatibility style
        // cl::compatibility::make_kernel<std::vector<float, svmalloc_t>&,
        //                                std::vector<float, svmalloc_t>&,
        //                                std::vector<float, svmalloc_t>&
        //                                >
        //                                triad{cl::Kernel{program, "triad"}};
        // triad(q_args, A, B, C).wait();
        auto triad_kernel = cl::KernelFunctor<
                                  std::vector<float, svmalloc_t>&,
                                  std::vector<float, svmalloc_t>&,
                                  std::vector<float, svmalloc_t>&
                                 >{program, "triad"};
        triad_kernel(q_args, A, B, C, err).wait();
        if(err!=CL_SUCCESS) std::cout << "Kernel execution error\n";
        gateway.try_put((tbb::tick_count::now()-t).seconds());
        gateway.release_wait();
      }};
  }
private:
  std::thread asyncThread;
};

int main(int argc, const char* argv[]) {
  int nth = 4;
  vsize = 100000000;
  float ratio = 0.5;
  float alpha = 0.5;
  opencl_initialize();

  tbb::task_scheduler_init init{nth};
  tbb::flow::graph g;

  bool n = false;
  tbb::flow::source_node<float> in_node{g, [&](float& offload_ratio)->bool {
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
            C[i] = A[i] + alpha * B[i];
        }
      );
      return (tbb::tick_count::now() - t).seconds();
  }};

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
      std::vector<float> CGold(vsize);
      auto t = tbb::tick_count::now();
      std::transform(A.begin(),
                     A.end(),
                     B.begin(),
                     CGold.begin(),
                     [&](float a, float b)->float{return a+alpha*b;});
      double ts = (tbb::tick_count::now() - t).seconds();
      #ifdef VERBOSE
      if(vsize<100){
          std::cout << "Results: " << '\n';
          for (int i = 0; i < vsize; i++) {
            std::cout << C[i] << ", ";
          }
          std::cout<< '\n';
      }
      #endif
      // Compare golden triad with heterogeneous triad
      if (!std::equal(C.begin(), C.end(), CGold.begin()))
        std::cout << "Error!!\n";
      else
        std::cout << "Heterogenous computation correct.\n";

      std::cout << "Time serial cpu: " << ts << " seconds" << '\n';
      std::cout << "Time cpu: " << std::get<1>(times) << " seconds" << '\n';
      std::cout << "Time gpu: " << std::get<0>(times) << " seconds" << '\n';
  }};

  tbb::flow::make_edge(in_node, a_node);
  tbb::flow::make_edge(in_node, cpu_node);
  tbb::flow::make_edge(a_node, tbb::flow::input_port<0>(node_join));
  tbb::flow::make_edge(cpu_node, tbb::flow::input_port<1>(node_join));
  tbb::flow::make_edge(node_join, out_node);

  in_node.activate();
  g.wait_for_all();

  return 0;
}

/*Output:
./fig_18_11-svm
Number of platforms: 2
Platform name: Intel(R) CPU Runtime for OpenCL(TM) Applications
No GPU found in this platform
Platform name: Intel(R) OpenCL HD Graphics
Device name: Intel(R) Gen9 HD Graphics NEO with 24 compute units
OpenCL version: OpenCL C 2.0
SVM FINE GRAIN BUFFER supported!
SVM COARSE GRAIN BUFFER supported!
SVM FINE GRAIN SYSTEM supported!
SVM ATOMICS supported!
Heterogenous computation correct.
Time serial cpu: 0.106481 seconds
Time cpu: 0.0806433 seconds
Time gpu: 0.0826025 seconds
*/
