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
#include "tbb/flow_graph_opencl_node.h"
#include "tbb/tick_count.h"

//using namespace tbb;
//using namespace tbb::flow;
//using namespace std;

int main(int argc, const char* argv[]) {

tbb::flow::graph g;
//Source node:
int n=0;
using buffer_t = tbb::flow::opencl_buffer<cl_char>;
tbb::flow::source_node<buffer_t> in_node(g,[&](buffer_t &a) {
  if(n > 0) return false;
  else {
    std::cout << "Hello ";
    char str[] = "OpenCL_Node\n";
    a = buffer_t{sizeof(str)};
    std::copy_n( str, sizeof(str), a.begin() );
    n++;
    return true;
  }
},false);

//GPU node:
tbb::flow::opencl_program<> program(std::string("example.cl"));
tbb::flow::opencl_node<std::tuple<buffer_t>>
   gpu_node(g, program.get_kernel("cl_print"));
//gpu_node.set_range({{1}});

// gpu_node.set_range({{1,1,1},{1,1,1}}); //explicit
//gpu_node.set_range({{1,1,1},{0,0,0}}); //explicit with local (work_group size) automatic
//gpu_node.set_range({{1},{0,0,0}}); //valid
//gpu_node.set_range({{1},{1}}); //valid
//gpu_node.set_range({{1,-1,-1},{0}}); //valid
//gpu_node.set_range({{1,0,0},{0}}); //INVALID
std::list<int> lg = { 1, 1, 1};
std::list<int> ll = { 1, 1, 1};
//std::list<std::list<int>> l = { lg, ll}; INVALID to pass l
//gpu_node.set_range({ lg, ll}); //valid

std::vector<int> vg = { 1, 1, 1};
std::vector<int> vl = { 1, 1, 1};
//std::vector<std::vector<int>> v ={vg,vl}; INVALID to pass v
//gpu_node.set_range({vg,vl});
gpu_node.set_range({vg,ll});

//Output node:
tbb::flow::function_node<buffer_t> out_node{g, tbb::flow::unlimited,
  [](buffer_t const & m){
    char *str = (char*) m.begin();
    std::cout << "Bye! Received from: "<< str;
  }
};

//Edges:
tbb::flow::make_edge(in_node,gpu_node);
tbb::flow::make_edge(gpu_node, out_node);

//Run!
in_node.activate();
g.wait_for_all();
}
