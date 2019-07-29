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

#include <vector>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <random>
#include <cstdio>

#include <tbb/flow_graph_opencl_node.h>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/task_scheduler_init.h>
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

  int nth = 4;
  size_t vsize = 100000000;
  float ratio = 0.5;
  float alpha = 0.5;
  tbb::tick_count t0_p;

  tbb::task_scheduler_init init{nth};
  tbb::flow::graph g;

  using buffer_f = tbb::flow::opencl_buffer<cl_float>;
  buffer_f Adevice{vsize};
  buffer_f Bdevice{vsize};
  buffer_f Cdevice{vsize};
  float* Ahost = Adevice.data();
  float* Bhost = Bdevice.data();
  float* Chost = Cdevice.data();

  // Initialize random number generator
  std::random_device seed;    // Random device seed
  std::mt19937 mte{seed()};   // mersenne_twister_engine
  std::uniform_int_distribution<> uniform{0, 256};
  // Initialize A and B
  std::generate(Ahost, Ahost+vsize, [&]{return uniform(mte);});
  std::generate(Bhost, Bhost+vsize, [&]{return uniform(mte);});

  bool n = false;
  tbb::flow::source_node<float> in_node{g, [&](float& offload_ratio) {
    if(n) return false;
    offload_ratio = ratio;
    n = true;
    return true;
  },false};

  using NDRange = std::array<size_t, 1> ; //1D array of size_t
  using tuple_cpu = std::tuple<float*, float*, float*, size_t>;
  using mfn_t = tbb::flow::multifunction_node<float, std::tuple<buffer_f,buffer_f,buffer_f,NDRange,tuple_cpu> >;
  mfn_t dispatch_node{g,
    tbb::flow::unlimited,
    [&](const float& offload_ratio, mfn_t::output_ports_type& ports ){
    t0_p =tbb::tick_count::now();
    // Messages for the GPU
    std::get<0>(ports).try_put(Adevice);
    std::get<1>(ports).try_put(Bdevice);
    std::get<2>(ports).try_put(Cdevice);
    std::get<3>(ports).try_put({static_cast<size_t>(ceil(vsize*offload_ratio))});

    tuple_cpu cpu_vectors;
    std::get<0>(cpu_vectors) = Ahost;
    std::get<1>(cpu_vectors) = Bhost;
    std::get<2>(cpu_vectors) = Chost;
    std::get<3>(cpu_vectors) = ceil(vsize*offload_ratio);
    // Message for the CPU
    std::get<4>(ports).try_put(cpu_vectors);
  }};

  gpu_device_selector gpu_selector;
  //tbb::flow::opencl_program<> program(tbb::flow::opencl_program_type::SOURCE, std::string("triad.cl"));
  tbb::flow::opencl_program<> program{std::string{"triad.cl"}};
  using tuple_gpu = std::tuple<buffer_f, buffer_f, buffer_f, NDRange>;
  tbb::flow::opencl_node<tuple_gpu> gpu_node{g,
                                          program.get_kernel("triad"),
                                          gpu_selector};
  gpu_node.set_args(tbb::flow::port_ref<0, 2>, alpha);
  gpu_node.set_range(tbb::flow::port_ref<3>); //NDRange comes in the last port


  tbb::flow::function_node<tuple_cpu, float *> cpu_node{g,
    tbb::flow::unlimited,
    [&](tuple_cpu cpu_vectors) {
      float* Ahost = std::get<0>(cpu_vectors);
      float* Bhost = std::get<1>(cpu_vectors);
      float* Chost = std::get<2>(cpu_vectors);
      size_t start = std::get<3>(cpu_vectors);
      // Parallel execution on the CPU
      parallel_for(tbb::blocked_range<size_t>{start, vsize},
         [&](const tbb::blocked_range<size_t>& r){
           for (size_t i = r.begin(); i < r.end(); ++i)
             Chost[i] = Ahost[i] + alpha * Bhost[i];
          });
      return Chost;
  }};

  using join_t = tbb::flow::join_node<std::tuple<buffer_f,float*>,
                                      tbb::flow::queueing>;
  join_t node_join{g};

  tbb::flow::function_node<join_t::output_type> out_node{g,
    tbb::flow::unlimited,
    [&](const join_t::output_type& m){
      double t_p = (tbb::tick_count::now() - t0_p).seconds();

      // Serial execution
      //std::unique_ptr<float[]> CGold{new float[vsize]};
      std::vector<float> CGold(vsize);
      tbb::tick_count t = tbb::tick_count::now();
      std::transform(Ahost, Ahost + vsize, Bhost, CGold.begin(),
                [&](float a, float b)->float{return a+alpha*b;});
      double t_s = (tbb::tick_count::now() - t).seconds();

      std::tie (Cdevice, Chost) = m; //unpack the tuple m

      if ( ! std::equal(Chost, Chost + vsize, Cdevice.begin()))
        std::cout << "Error!!\n";
      else
        std::cout << "Chost and Cdevice are equal.\n";

      //both views of the array point to the same buffer
      std::cout << "Chost points to " << Chost << '\n';
      std::cout << "Cdevice.begin() points to " << Cdevice.begin() << '\n';
      std::cout << "Cdevice.data() points to " << Cdevice.data() << '\n';

      #ifdef VERBOSE
          std::cout << "Results: " << '\n';
          for (size_t i = 0; i < vsize; i++) {
            std::cout <<Chost[i]<<", ";
          }
          std::cout<< '\n';
      #endif
      // Check correctness:
      if ( ! std::equal (Chost, Chost+vsize, CGold.begin()))
        std::cout << "Error!!\n";
      else
        std::cout << "Heterogenous computation correct.\n";

      if (std::equal(Cdevice.begin(), Cdevice.end(), CGold.begin()))
          std::cout << "Second validation.\n";
      if (std::equal(Cdevice.data(), Cdevice.data()+vsize, CGold.begin()))
          std::cout << "Third validation.\n";

      std::cout << "Serial: "   << t_s << ", ";
      std::cout << "Parallel: " << t_p << ", ";
      std::cout << "Speed-up: " << t_s/t_p << std::endl;
  }};

  // in_node -> dispatch_node
  tbb::flow::make_edge(in_node, dispatch_node);
  // dispatch_node -> gpu_node
  tbb::flow::make_edge(tbb::flow::output_port<0>(dispatch_node),
                       tbb::flow::input_port<0>(gpu_node));
  tbb::flow::make_edge(tbb::flow::output_port<1>(dispatch_node),
                       tbb::flow::input_port<1>(gpu_node));
  tbb::flow::make_edge(tbb::flow::output_port<2>(dispatch_node),
                       tbb::flow::input_port<2>(gpu_node));
  tbb::flow::make_edge(tbb::flow::output_port<3>(dispatch_node),
                       tbb::flow::input_port<3>(gpu_node));
  // dispatch_node -> cpu_node
  tbb::flow::make_edge(tbb::flow::output_port<4>(dispatch_node),
                       cpu_node);
  // gpu_node -> node_join
  tbb::flow::make_edge(tbb::flow::output_port<2>(gpu_node),
                       tbb::flow::input_port<0>(node_join));
  // cpu_node -> node_join
  tbb::flow::make_edge(cpu_node, tbb::flow::input_port<1>(node_join));
  // node_join -> out_node
  tbb::flow::make_edge(node_join, out_node);

  tbb::tick_count t = tbb::tick_count::now();
  in_node.activate();
  g.wait_for_all();

  std::cout << "Total execution time= "
            << (tbb::tick_count::now() - t).seconds()
            << " seconds." << '\n';
  return 0;
}

/* Output:
asenjo:Vector_Add_Parallelized>./triad_oclNode 4 100000000 0.8
Running on Intel(R) HD Graphics 530
Heterogenous computation correct.
Serial: 0.377434, Parallel: 0.168683, Speed-up: 2.23753
*/
