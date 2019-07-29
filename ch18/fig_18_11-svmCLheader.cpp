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

#include <cstdio>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <cmath>
#include <tbb/flow_graph.h>
#include <tbb/tick_count.h>
#include <tbb/compat/thread>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/task_scheduler_init.h>

#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

int vsize;
float* A;                       // Host and Device view of A, B and C arrays
float* B;                       //
float* C;                       //
cl_context context;
cl_command_queue commands;
cl_kernel kernel;

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

void OpenCL_Initialize(){
  int err;
  // Get the number of platforms
  cl_uint num_platforms = 0;
  cl_platform_id* platform_ids = NULL;
  err = clGetPlatformIDs(0, NULL, &num_platforms);
  if (err==CL_SUCCESS && num_platforms)
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
  }
  num_devices = 1;
  std::cerr << "Device's name: " << device_name << " with "<< computeUnits << " computes Units" << '\n';
  char device_version[50];
  err = clGetDeviceInfo(device_id, CL_DEVICE_OPENCL_C_VERSION, sizeof(char)*50, &device_version, NULL);
  if (err != CL_SUCCESS) {
    fprintf(stderr, "Device version not available");
  }
  std::cerr << "OpenCL version: " << device_version  << std::endl;
  cl_device_svm_capabilities device_svm;
  err = clGetDeviceInfo(device_id, CL_DEVICE_SVM_CAPABILITIES, sizeof(cl_device_svm_capabilities), &device_svm, NULL);
  if (err != CL_SUCCESS) {
    fprintf(stderr, "SVM capabilities not available");
    exit(1);
  }

  if (device_svm & CL_DEVICE_SVM_FINE_GRAIN_BUFFER)
    fprintf(stderr, "SVM FINE GRAIN BUFFER supported!\n");

  if (device_svm & CL_DEVICE_SVM_COARSE_GRAIN_BUFFER)
    fprintf(stderr, "SVM COARSE GRAIN BUFFER supported!\n");

  if (device_svm & CL_DEVICE_SVM_FINE_GRAIN_SYSTEM)
    fprintf(stderr, "SVM FINE GRAIN SYSTEM supported!\n");

  if (device_svm & CL_DEVICE_SVM_ATOMICS)
    fprintf(stderr, "SVM ATOMICS supported!\n");

  // Create the context
  cl_context context = clCreateContext(NULL, num_devices, &device_id, NULL, NULL, &err);
  if (err != CL_SUCCESS){
    fprintf(stderr, "Context couldn't be created!\n");
    clReleaseContext(context);
    exit(1);
  }
  // Create the command queue
  commands = clCreateCommandQueue(context, device_id, 0, &err);
  if (err != CL_SUCCESS){
    printf("Error: Failed to create a command commands!\n");
    clReleaseContext(context);
    clReleaseCommandQueue(commands);
    exit(1);
  }
  // Create the compute program from the source buffer
  const std::string& kernel_name = read_kernel_source("triad.cl");
  const char* KernelSource = kernel_name.c_str();
  cl_program program = clCreateProgramWithSource(context, 1, (const char **)&KernelSource, NULL, &err);
  if (err!=CL_SUCCESS) {
    printf("Error: Failed to create the GPU program!\n");
    clReleaseContext(context);
    clReleaseCommandQueue(commands);
    clReleaseProgram(program);
    exit(1);
  }
  // Build the program executable
  const char options[] = "-cl-std=CL2.0";
  err = clBuildProgram(program, 0, NULL, options, NULL, NULL);
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
    clReleaseContext(context);
    clReleaseCommandQueue(commands);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    exit(1);
  }
  clReleaseProgram(program);

  // Create the input and output arrays
  A = (float*)clSVMAlloc(context, CL_MEM_READ_WRITE | CL_MEM_SVM_FINE_GRAIN_BUFFER, sizeof(float) * vsize, 0);
  B = (float*)clSVMAlloc(context, CL_MEM_READ_WRITE | CL_MEM_SVM_FINE_GRAIN_BUFFER, sizeof(float) * vsize, 0);
  C = (float*)clSVMAlloc(context, CL_MEM_READ_WRITE | CL_MEM_SVM_FINE_GRAIN_BUFFER, sizeof(float) * vsize, 0);
  for(int i = 0; i < vsize; i++){
    A[i] = i;
    B[i] = i;
  }
}

void OpenCLRelease(){
  clSVMFree(context, A);
  clSVMFree(context, B);
  clSVMFree(context, C);
  clReleaseContext(context);
  clReleaseCommandQueue(commands);
  clReleaseKernel(kernel);
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
    void run(float offloadRatio, gateway_type& gateway) {
        gateway.reserve_wait();
        asyncThread=std::thread ([&,offloadRatio]()
        {
           tbb::tick_count t = tbb::tick_count::now();
           // Set the arguments to our compute kernel
           //
           cl_int err = 0;
           err  = clSetKernelArgSVMPointer(kernel, 0, A);
           err |= clSetKernelArgSVMPointer(kernel, 1, B);
           err |= clSetKernelArgSVMPointer(kernel, 2, C);

           if (err != CL_SUCCESS){
               printf("Error: Failed to set kernel arguments! %d\n", err);
               exit(1);
           }
           // Execute the kernel over a portion of the vector range
           size_t global = ceil(vsize*offloadRatio);
           err = clEnqueueNDRangeKernel(commands, kernel, 1, NULL, &global, NULL, 0, NULL, NULL);
           if (err)
           {
               printf("Error: Failed to execute kernel!\n");
               exit(1);
           }
           // Wait for the command queue
           clFinish(commands);
           gateway.try_put((tbb::tick_count::now()-t).seconds());
           gateway.release_wait();
        });
      }
    private:
        std::thread asyncThread;
};

int main(int argc, const char* argv[]) {

  int nth = 4;
  vsize = 100000000;
  float ratio = 0.5;
  float alpha = 0.5;

  tbb::task_scheduler_init init(nth);

  tbb::flow::graph g;

  OpenCL_Initialize();

  int n=0;
  tbb::flow::source_node<float> input_node(g,[&](float &a)->bool {
    if(n>0) return false;
    a=ratio;
    n++;
    return true;
  },false);

  tbb::flow::function_node<float,double> cpu_node(g,tbb::flow::unlimited,[&](float offloadRatio) -> double {

    tbb::tick_count t=tbb::tick_count::now();
    tbb::parallel_for(tbb::blocked_range<size_t>(ceil(vsize*offloadRatio), vsize),//ceil(vsize/2.0)PROBAR.
    [&](const tbb::blocked_range<size_t>& r){
      for (size_t i = r.begin(); i < r.end(); ++i)
        C[i]=A[i]+alpha*B[i];
    });
    return (tbb::tick_count::now() - t).seconds();
  });

  using async_node_type = tbb::flow::async_node<float, double>;
  using gateway_type = async_node_type::gateway_type;
  AsyncActivity asyncAct;
  async_node_type a_node (g, tbb::flow::unlimited,
    [&asyncAct](const float& offloadRatio, gateway_type& gateway) {
      asyncAct.run(offloadRatio,gateway);
    });

  using join_t = tbb::flow::join_node< std::tuple<double,double>, tbb::flow::queueing >;
  join_t node_join(g);

  tbb::flow::function_node<join_t::output_type> out_node(g, tbb::flow::unlimited,
    [&](const join_t::output_type &times){
    // Serial execution
    std::vector<float>CGold(vsize);
    tbb::tick_count t = tbb::tick_count::now();
    transform(A,
              A + vsize,
              B,
              CGold.begin(),
              [&](float a, float b)->float{return a+alpha*b;});
    double ts = (tbb::tick_count::now() - t).seconds();

    #ifdef VERBOSE
        std::cout << "Results: " << '\n';
        for (size_t i = 0; i < vsize; i++) {
          std::cout <<C[i]<<", ";
        }
        std::cout<< '\n';
    #endif
    // using default comparison:
    if ( std::equal (C, C+vsize, CGold.begin()))
      std::cout << "Heterogenous computation correct.\n";
    else
      std::cout << "Errors in the heterogeneous computation.\n";

    std::cout << "Time serial cpu: " << ts << " seconds" << "\n";
    std::cout << "Time cpu: " << std::get<1>(times) << " seconds" << "\n";
    std::cout << "Time gpu: " << std::get<0>(times) << " seconds" << std::endl;
  });

  tbb::flow::make_edge(input_node,a_node);
  tbb::flow::make_edge(input_node,cpu_node);
  tbb::flow::make_edge(a_node,tbb::flow::input_port<0>(node_join));
  tbb::flow::make_edge(cpu_node,tbb::flow::input_port<1>(node_join));
  tbb::flow::make_edge(node_join,out_node);

  input_node.activate();
  g.wait_for_all();
  OpenCLRelease();

  return 0;
}

/*Output:
./fig_18_11-svmCLheader 4 100000000 0.2
Device's name: Intel(R) Gen9 HD Graphics NEO with 24 computes Units
OpenCL version: OpenCL C 2.0
SVM FINE GRAIN BUFFER supported!
SVM COARSE GRAIN BUFFER supported!
SVM FINE GRAIN SYSTEM supported!
SVM ATOMICS supported!
Heterogenous computation correct.
Time serial cpu: 0.105485 seconds
Time cpu: 0.0845081 seconds
Time gpu: 0.0988554 seconds
*/
