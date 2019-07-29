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

#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

int vsize;
float* Ahost;                       // Host view of A, B and C arrays
float* Bhost;                       //
float* Chost;                       //
cl_mem Adevice;                     // Device view of A, B and C arrays
cl_mem Bdevice;                     //
cl_mem Cdevice;                     //
cl_command_queue commands;          // OpenCL command queue
cl_kernel kernel;                   // OpenCL kernel

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
  int err;
  // Get the number of platforms
  cl_uint num_platforms = 0;
  cl_platform_id* platform_ids = NULL;
  err = clGetPlatformIDs(0, NULL, &num_platforms);
  if (err==CL_SUCCESS && num_platforms>0)
    platform_ids = new cl_platform_id[num_platforms];
  else {
    fprintf(stderr, "No platforms were found.\n");
    exit(1);
  }
  err = clGetPlatformIDs(num_platforms, platform_ids, NULL);
  if (err != CL_SUCCESS){
    fprintf(stderr, "No platforms were found.\n");
    delete[] platform_ids;
    exit(1);
  }
  // Find a GPU device in one of the platforms
  cl_uint num_max_devices = 1;
  cl_uint num_devices = 0;
  cl_device_id device_id;
  bool found = false;
  for(cl_uint i = 0; i < num_platforms; i++){
    err = clGetDeviceIDs(platform_ids[i], CL_DEVICE_TYPE_GPU, num_max_devices, &device_id, &num_devices);
    if (err == CL_SUCCESS) {
      found = true;
      break;
    }
  }
  delete[] platform_ids;
  if(!found){
    fprintf(stderr, "No GPU device found\n");
    exit(1);
  }
  int computeUnits;
  char device_name[50];
  err = clGetDeviceInfo(device_id, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &computeUnits, NULL);
  err |= clGetDeviceInfo(device_id, CL_DEVICE_NAME, sizeof(char)*50, &device_name, NULL);
  if (err != CL_SUCCESS){
    fprintf(stderr, "Compute units info or device name not available!\n");
    exit(1);
  }
  num_devices = 1;
  std::cerr << "Device's name: " << device_name << " with "<< computeUnits << " computes Units" << '\n';
  // Create the context
  cl_context context = clCreateContext(NULL, num_devices, &device_id, NULL, NULL, &err);
  if (err != CL_SUCCESS){
    fprintf(stderr, "Context couldn't be created!\n");
    exit(1);
  }
  // Create the command queue
  commands = clCreateCommandQueue(context, device_id, 0, &err);
  if (!commands){
    printf("Error: Failed to create a command commands!\n");
    exit(1);
  }
  // Create the compute program from the source buffer
  const std::string& kernel_name = read_kernel_source("triad.cl");
  const char* KernelSource = kernel_name.c_str();
  cl_program program = clCreateProgramWithSource(context, 1, (const char **)&KernelSource, NULL, &err);
  if (!program) {
    printf("Error: Failed to create the GPU program!\n");
    exit(1);
  }
  // Build the program executable
  err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
  if (err != CL_SUCCESS){
    size_t len;
    char buffer[2048];
    printf("Error: Failed to build GPU executable!\n");
    clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
    printf("%s\n", buffer);
    exit(1);
  }
  // Create the GPU kernel
  kernel = clCreateKernel(program, "triad", &err);
  if (!kernel || err != CL_SUCCESS){
    printf("Error: Failed to create GPU kernel!\n");
    exit(1);
  }
  clReleaseProgram(program);

  // Create the input and output arrays in device memory
  Adevice = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, sizeof(float) * vsize, NULL, NULL);
  Bdevice = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, sizeof(float) * vsize, NULL, NULL);
  Cdevice = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, sizeof(float) * vsize, NULL, NULL);
  if (!Adevice || !Bdevice || !Cdevice )
  {
    printf("Error: Failed to allocate device memory!\n");
    exit(1);
  }
  clReleaseContext(context);
  // Map device view of arrays into host view Ahost and Bhost
  Ahost=(float*)clEnqueueMapBuffer(commands, Adevice, CL_TRUE, CL_MAP_WRITE | CL_MAP_READ, 0, sizeof(float) * vsize, 0, NULL, NULL, &err);
  Bhost=(float*)clEnqueueMapBuffer(commands, Bdevice, CL_TRUE, CL_MAP_WRITE | CL_MAP_READ, 0, sizeof(float) * vsize, 0, NULL, NULL, &err);
  Chost=(float*)clEnqueueMapBuffer(commands, Cdevice, CL_TRUE, CL_MAP_WRITE | CL_MAP_READ, 0, sizeof(float) * vsize, 0, NULL, NULL, NULL);

  for(int i = 0; i < vsize; i++){
    Ahost[i] = i;
    Bhost[i] = i;
  }
  err = clEnqueueUnmapMemObject(commands, Adevice, Ahost, 0, NULL, NULL);
  err|= clEnqueueUnmapMemObject(commands, Bdevice, Bhost, 0, NULL, NULL);
  if (err != CL_SUCCESS){
    printf("Error: Failed to unmap Ahost or Bhost!\n");
    exit(1);
  }
}

void OpenCLRelease(){
  clReleaseMemObject(Adevice);
  clReleaseMemObject(Bdevice);
  clReleaseMemObject(Cdevice);
  clReleaseKernel(kernel);
  clReleaseCommandQueue(commands);
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
        int err = 0;
        err  = clSetKernelArg(kernel, 0, sizeof(cl_mem), &Adevice);
        err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &Bdevice);
        err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &Cdevice);
        if (err != CL_SUCCESS){
            printf("Error: Failed to set kernel arguments! %d\n", err);
            exit(1);
        }
        // Execute the kernel over a portion of the vector range
        size_t global = ceil(vsize*offload_ratio);
        err = clEnqueueNDRangeKernel(commands, kernel, 1, NULL, &global, NULL, 0, NULL, NULL);
        if (err != CL_SUCCESS){
            printf("Error: Failed to execute kernel!\n");
            exit(1);
        }
        // Wait for the command queue
        clFinish(commands);
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
      std::vector<float>CGold(vsize);
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
  tbb::flow::make_edge(in_node, a_node);
  tbb::flow::make_edge(in_node, cpu_node);
  tbb::flow::make_edge(a_node, tbb::flow::input_port<0>(node_join));
  tbb::flow::make_edge(cpu_node, tbb::flow::input_port<1>(node_join));
  tbb::flow::make_edge(node_join, out_node);

  in_node.activate();
  g.wait_for_all();
  OpenCLRelease();

  return 0;
} // End of main()!

/*Output:
./fig_18_11-CLheader
Device's name: Intel(R) Gen9 HD Graphics NEO with 24 computes Units
Heterogenous computation correct.
Time serial cpu: 0.107458 seconds
Time cpu: 0.0877835 seconds
Time gpu: 0.0614393 seconds
*/
