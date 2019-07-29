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
#include <numeric>
#include "tbb/flow_graph.h"
#include "tbb/flow_graph_opencl_node.h"
#include "tbb/tick_count.h"

using namespace tbb;
using namespace tbb::flow;
using namespace std;

int main(int argc, const char* argv[]) {

  int vsize = 4;

  tbb::flow::graph g;

  using buffer_f = tbb::flow::opencl_buffer<cl_int>;
  buffer_f Adevice(vsize);
  int* Ahost=Adevice.data();
  std::iota(Ahost, Ahost+vsize, 0); // 0, 1, 2, 3, ...

  //GPU node:
  tbb::flow::opencl_program<> program(std::string("inc.cl"));
  std::atomic<int> deviceNum(0);
  opencl_node<tuple<buffer_f>> gpu_node(g, program.get_kernel("cl_inc"), [&deviceNum](auto &f){
    auto d = *(f.devices().begin() + deviceNum++ % f.devices().size());
    std::cout << "Running on "<< d.name() << std::endl;
    return d;
  });
  gpu_node.set_range({{vsize}});
  gpu_node.set_args(port_ref<0>);
  for(int i=0;i<3;i++){
    std::cout<< "Iteration: " << i << '\n';
    tbb::flow::input_port<0>(gpu_node).try_put(Adevice);
  }

  g.wait_for_all();

  int* AGold=new int[vsize];
  std::iota(AGold, AGold + vsize, 3); // 3, 4, 5, 6, ...
  if (! std::equal (Adevice.begin(),Adevice.end(),AGold))
    std::cout << "Errors in the heterogeneous computation.\n";
#ifdef VERBOSE
    std::cout << "Results: " << '\n';
    for (size_t i = 0; i < vsize; i++) {
      std::cout <<Ahost[i]<<", ";
    }
    std::cout<< '\n';
#endif
}
