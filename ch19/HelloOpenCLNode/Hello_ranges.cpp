/*********************************************************
**********************************************************
*Presentation of flow_graph_opencl_node and the Heterogeneous capabilities in FlowGraph

-Build instructions:

Linux:   g++ Hello_OCLNode.cpp -o  -ltbb -std=c++11 -lOpenCL -O3
MacOS:   g++-6 Hello_OCLNode.cpp -o example -ltbb -std=c++11 -framework OpenCL -w -O3

-Execute instructions:
./example
***************************************************************/
#define TBB_PREVIEW_FLOW_GRAPH_NODES 1
#define TBB_PREVIEW_FLOW_GRAPH_FEATURES 1

#include <cstdio>
#include "tbb/flow_graph.h"
#include "tbb/flow_graph_opencl_node.h"
#include "tbb/tick_count.h"

using namespace tbb;
using namespace tbb::flow;
using namespace std;

const int NUM_RAND = 256;
int RandomNumber () { return (std::rand()%NUM_RAND); }
tbb::tick_count t0_p;

class gpu_device_selector1 {
public:
  template<typename DeviceFilter> opencl_device operator()(opencl_factory<DeviceFilter> &f) {
    //std::cout << "Available devices for kernel execution:\n";
    int i = 0;
    std::for_each( f.devices().cbegin(), f.devices().cend(),
      [&](const tbb::flow::opencl_device &d) {
      //std::cout << i++ << ".- Device: " << d.name() << std::endl;
    });
    tbb::flow::opencl_device d=*(++f.devices().cbegin());
    //std::cout << "Running on " << d.name() << '\n';
    return d;
  }
};

class gpu_device_selector2 {
public:
  template<typename DeviceFilter> opencl_device operator()(opencl_factory<DeviceFilter> &f) {
    //tbb::flow::opencl_device_list::const_iterator it = std::find_if(
    auto it = std::find_if(
      f.devices().cbegin(), f.devices().cend(),
      [](const tbb::flow::opencl_device &d) {
        if(  d.type() == CL_DEVICE_TYPE_GPU) {
          std::cout << "Found GPU!" << std::endl;
          return true;
        }
        return false;
      });

      if (it == f.devices().cend()) {
        std::cout << "Info: could not find any GPU devices. Choosing the first available device (default behaviour)." << std::endl;
        return {*f.devices().cbegin()};
      }
      it++;
      std::cout << "Running OpenCL code on "<< (*it).name() << std::endl;
      return {*it};
    }
  };

int main(int argc, const char* argv[]) {

  int rows = 4;
  int cols = 4;
  int vsize = rows * cols;

  tbb::flow::graph g;

  using buffer_f = tbb::flow::opencl_buffer<cl_float>;
  buffer_f Adevice(vsize);
  buffer_f Bdevice(vsize);
  buffer_f Cdevice(vsize);
  float* Ahost=Adevice.data();
  float* Bhost=Bdevice.data();
  std::generate(Ahost, Ahost+vsize, RandomNumber);
  std::generate(Bhost, Bhost+vsize, RandomNumber);

  //GPU node:
  gpu_device_selector2 gpu_selector;
  tbb::flow::opencl_program<> program(std::string("printranges.cl"));
  using tuple_gpu = std::tuple<buffer_f,buffer_f,buffer_f>;
  opencl_node<tuple_gpu>
      gpu_node(g, program.get_kernel("cl_print"), gpu_selector);
  gpu_node.set_range({{rows,cols}});//,{rows/2,cols/2}});
  gpu_node.set_args(port_ref<0,2>,rows);
  tbb::flow::input_port<0>(gpu_node).try_put(Adevice);
  tbb::flow::input_port<1>(gpu_node).try_put(Bdevice);
  tbb::flow::input_port<2>(gpu_node).try_put(Cdevice);
  //tbb::flow::input_port<3>(gpu_node).try_put({ rows });

  g.wait_for_all();

  float* Chost=Cdevice.data();
  float* CGold=new float[vsize];
    std::transform(Ahost, Ahost + vsize, Bhost, CGold,std::plus<float>());

  #ifdef VERBOSE
      std::cout << "Results: " << '\n';
      for (size_t i = 0; i < vsize; i++) {
        std::cout <<Chost[i]<<", ";
      }
      std::cout<< '\n';
  #endif
  // using default comparison:
  if (! std::equal (Chost,Chost+vsize,CGold))
    std::cout << "Errors in the heterogeneous computation.\n";

}
