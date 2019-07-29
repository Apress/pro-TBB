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

class gpu_device_selector2 {
public:
  template<typename DeviceFilter> opencl_device operator()(opencl_factory<DeviceFilter> &f) {
    std::cout << "Available devices for kernel execution:\n";
    int i = 0;
    std::for_each( f.devices().cbegin(), f.devices().cend(),
      [&](const tbb::flow::opencl_device &d) {
         std::cout << i++ << ".- Device: " << d.name() << std::endl;
      });
    tbb::flow::opencl_device d=*(++f.devices().cbegin());
    std::cout << "Running on " << d.name() << '\n';
    return d;
  }
};

class gpu_device_selector1 {
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
//      it++;
      std::cout << "Running OpenCL code on "<< (*it).name() << std::endl;
      return {*it};
    }
  };

  class gpu_device_selector3 {
  public:
    template<typename DeviceFilter> opencl_device operator()(opencl_factory<DeviceFilter> &f) {
      //tbb::flow::opencl_device_list::const_iterator it = std::find_if(
      auto it = std::find_if(f.devices().cbegin(), f.devices().cend(),
        [](const tbb::flow::opencl_device &d) {
          return  d.type() == CL_DEVICE_TYPE_GPU;
        });

        if (it == f.devices().cend()) {
          std::cout << "Info: could not find any GPU devices." << std::endl;
          return {*f.devices().cbegin()};
        }
        std::cout << "Running OpenCL code on "<< it->name() << std::endl;
        return {*it};
      }
    };

int main(int argc, const char* argv[]) {

  graph g;

  //Source node:
  int n=0;
  using buffer_t = opencl_buffer<cl_char>;
  source_node<buffer_t> in_node(g,[&](buffer_t &a) {
    if(n > 0) return false;
    else {
      std::cout << "Hello ";
      char str[] = "OpenCL_Node\n";
      buffer_t b(sizeof(str));
      std::copy_n( str, sizeof(str), b.begin() );
      a=std::move(b);
      n++;
      return true;
    }
  },false);

  //GPU node:
  gpu_device_selector2 gpu_selector;
  opencl_program<> program( std::string("example.cl"));
  opencl_node<tuple<buffer_t>>
  //  gpu_node(g, program.get_kernel("cl_print"), gpu_selector);
    gpu_node(g, program.get_kernel("cl_print"),[]
     (auto &f) { //polymorphic lambdas as of C++14
       return *(++f.devices().cbegin());
     });

    // gpu_node(g, program.get_kernel("cl_print"),[]
    //  (auto &f) { //polymorphic lambdas as of C++14
    //    std::cout << "Available devices:\n";
    //    int i = 0;
    //    std::for_each( f.devices().cbegin(), f.devices().cend(),
    //      [&](const tbb::flow::opencl_device &d) {
    //      std::cout << i++ << ".- Device: " << d.name() << std::endl;
    //    });
    //    tbb::flow::opencl_device d=*(++f.devices().cbegin());
    //    std::cout << "Running on " << d.name() << '\n';
    //    return d;
    //  });

  //std::array<unsigned int, 1> range{1};
  gpu_node.set_range({{1}});

  //Output node:
  tbb::flow::function_node<buffer_t> out_node{g, tbb::flow::unlimited,
    [](buffer_t const & m){
      char *str = (char*) m.begin();
      std::cout << "Bye! Received from: "<< str;
    }
  };

  //Edges:
  make_edge(in_node,gpu_node);
  make_edge(gpu_node, out_node);

  //Run!
  in_node.activate();
  g.wait_for_all();
}
