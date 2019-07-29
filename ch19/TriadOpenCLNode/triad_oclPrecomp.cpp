/*********************************************************
**********************************************************
*Example 3. Parallelization of simple_add of a vector in Heterogeneous systems.


Instructions to build and execute:

Build instructions:
Linux:   g++  FlowGraph_Vector_Add_Parallelized.cpp -o example_Vector_Add_Parallelized -ltbb -std=c++11 -lOpenCL -w -O3
MacOS:   g++  FlowGraph_Vector_Add_Parallelized.cpp -o example_Vector_Add_Parallelized -ltbb -std=c++11 -framework OpenCL -w -O3

Execute:

./example_Vector_Add_Parallelized
***************************************************************/
#define TBB_PREVIEW_FLOW_GRAPH_NODES 1
#define TBB_PREVIEW_FLOW_GRAPH_FEATURES 1

#include <vector>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <cstdio>

#include <tbb/flow_graph_opencl_node.h>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/task_scheduler_init.h>
#include <tbb/tick_count.h>

const int NUM_RAND = 256;
int RandomNumber () { return (std::rand()%NUM_RAND); }
tbb::tick_count t0_p;

class gpu_device_selector1 {
public:
  template<typename DeviceFilter>
  tbb::flow::opencl_device operator()(tbb::flow::opencl_factory<DeviceFilter> &f) {
    auto it = std::find_if(
      f.devices().cbegin(), f.devices().cend(),
      [](const tbb::flow::opencl_device &d) {
        return d.type() == CL_DEVICE_TYPE_GPU;});
    std::cout << "Running on "<< it->name() << std::endl;
    return *it;
  }
};

class gpu_device_selector {
public:
  template<typename DeviceFilter>
  tbb::flow::opencl_device operator()(tbb::flow::opencl_factory<DeviceFilter> &f) {
    return *(++f.devices().cbegin());//integrated GPU on macbook pro
    //return *(++(++f.devices().cbegin()));//discrete GPU on macbook pro
  }
};

int main(int argc, const char* argv[]) {

  int nth=2;
  int vsize = 10;
  float ratio=0.5;
  tbb::task_scheduler_init init(nth);

tbb::flow::graph g;

using buffer_f = tbb::flow::opencl_buffer<cl_float>;
buffer_f Adevice(vsize);
buffer_f Bdevice(vsize);
buffer_f Cdevice(vsize);
float* Ahost=Adevice.data();
float* Bhost=Bdevice.data();
float* Chost=Cdevice.data();

std::generate(Ahost, Ahost+vsize, RandomNumber);
std::generate(Bhost, Bhost+vsize, RandomNumber);

int  n=0;
tbb::flow::source_node<float> in_node(g,[&](float &offloadRatio)->bool {
  if(n>0) return false;
  offloadRatio=ratio;
  n++;
  return true;
},false);

using NDRange = std::array<uint, 1> ; //1D array of int
using tuple_cpu = std::tuple<float*, float*, float*, int>;
using mfn_t = tbb::flow::multifunction_node<float, std::tuple<buffer_f,buffer_f,buffer_f,NDRange,tuple_cpu> >;
mfn_t dispatch_node(g, tbb::flow::unlimited, [&]( const float& offloadRatio, mfn_t::output_ports_type &ports ) {

  t0_p =tbb::tick_count::now();
  // Messages for the GPU
  std::get<0>(ports).try_put(Adevice);
  std::get<1>(ports).try_put(Bdevice);
  std::get<2>(ports).try_put(Cdevice);
  std::get<3>(ports).try_put({ (uint) ceil(vsize*offloadRatio) });

  tuple_cpu cpu_vectors;
  std::get<0>(cpu_vectors)=Ahost;
  std::get<1>(cpu_vectors)=Bhost;
  std::get<2>(cpu_vectors)=Chost;
  std::get<3>(cpu_vectors)=ceil(vsize*offloadRatio);
  // Message for the CPU
  std::get<4>(ports).try_put(cpu_vectors);
});

gpu_device_selector1 gpu_selector;
//tbb::flow::opencl_program<> program(tbb::flow::opencl_program_type::PRECOMPILED, std::string("triad.clbin"));
tbb::flow::opencl_program<> program(tbb::flow::opencl_program_type::SPIR, std::string("triad.spir"));
using tuple_gpu = std::tuple<buffer_f,buffer_f,buffer_f, NDRange>;
tbb::flow::opencl_node<tuple_gpu>
   gpu_node(g, program.get_kernel("triad"), gpu_selector);
gpu_node.set_range(tbb::flow::port_ref<3>); //NDRange comes in the last port
gpu_node.set_args(tbb::flow::port_ref<0,2>, 0.5f);

tbb::flow::function_node<tuple_cpu,float *> cpu_node(g,tbb::flow::unlimited,[&](tuple_cpu cpu_vectors) {
  float* Ahost=std::get<0>(cpu_vectors);
  float* Bhost=std::get<1>(cpu_vectors);
  float* Chost=std::get<2>(cpu_vectors);
  int    start=std::get<3>(cpu_vectors);
  // Parallel execution
  parallel_for(tbb::blocked_range<size_t>(start, vsize),
     [&](const tbb::blocked_range<size_t>& r){
       for (size_t i = r.begin(); i < r.end(); ++i)
         Chost[i]=Ahost[i]+0.5*Bhost[i];
      });
  return Chost;
});

using join_t = tbb::flow::join_node<std::tuple<buffer_f,float*>, tbb::flow::queueing>;
join_t node_join(g);

//tbb::flow::function_node<join_t::output_type> out_node(g, tbb::flow::unlimited,
//  [&](const join_t::output_type &m){
tbb::flow::function_node<std::tuple<buffer_f,float*>> out_node(g, tbb::flow::unlimited,
  [&](const join_t::output_type &m){
    double t_p = (tbb::tick_count::now() - t0_p).seconds();

  // Serial execution
  float* CGold=new float[vsize];
  tbb::tick_count t = tbb::tick_count::now();
  std::transform(Ahost,
            Ahost + vsize,
            Bhost,
            CGold,
            [&](float a, float b)->float{return a+0.5*b;});
  double t_s = (tbb::tick_count::now() - t).seconds();

  //buffer_f Cdevice = std::get<0>(m);
  //float * Chost=std::get<1>(m);
  //buffer_f Cdevice;
  //float * Chost;
  std::tie (Cdevice, Chost) = m;

  if ( std::equal (Chost,Chost+vsize,Cdevice.begin()))
    std::cout << "Chost and Cdevice are equal.\n";
  else
    std::cout << "Chost and Cdevice differ.\n";

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
  // using default comparison:
  if ( std::equal (Chost,Chost+vsize,CGold))
    std::cout << "Heterogenous computation correct.\n";
  else
    std::cout << "Errors in the heterogeneous computation.\n";

  if ( std::equal (Cdevice.begin(),Cdevice.end(),CGold))
      std::cout << "Second validation.\n";
  if ( std::equal (Cdevice.data(),Cdevice.data()+vsize,CGold))
      std::cout << "Third validation.\n";

  std::cout << "Serial: " << t_s << ", ";
  std::cout << "Parallel: " << t_p << ", ";
  std::cout << "Speed-up: " << t_s/t_p << std::endl;
});

tbb::flow::make_edge(in_node,dispatch_node);

tbb::flow::make_edge(tbb::flow::output_port<0>(dispatch_node), tbb::flow::input_port<0>(gpu_node));
tbb::flow::make_edge(tbb::flow::output_port<1>(dispatch_node), tbb::flow::input_port<1>(gpu_node));
tbb::flow::make_edge(tbb::flow::output_port<2>(dispatch_node), tbb::flow::input_port<2>(gpu_node));
tbb::flow::make_edge(tbb::flow::output_port<3>(dispatch_node), tbb::flow::input_port<3>(gpu_node));
tbb::flow::make_edge(tbb::flow::output_port<4>(dispatch_node), cpu_node);

tbb::flow::make_edge(tbb::flow::output_port<2>(gpu_node),tbb::flow::input_port<0>(node_join));
tbb::flow::make_edge(cpu_node,tbb::flow::input_port<1>(node_join));

tbb::flow::make_edge(node_join,out_node);

tbb::tick_count mainStartTime = tbb::tick_count::now();
in_node.activate();
g.wait_for_all();

std::cout <<"Execution time= "<< (tbb::tick_count::now() - mainStartTime).seconds() <<" seconds"<<std::endl;
return 0;
}

/* Output:
asenjo:Vector_Add_Parallelized>./triad_oclNode 4 100000000 0.8
Running on Intel(R) HD Graphics 530
Heterogenous computation correct.
Serial: 0.377434, Parallel: 0.168683, Speed-up: 2.23753
*/
