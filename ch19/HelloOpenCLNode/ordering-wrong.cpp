#define TBB_PREVIEW_FLOW_GRAPH_NODES 1
#define TBB_PREVIEW_FLOW_GRAPH_FEATURES 1
#include "tbb/flow_graph_opencl_node.h"

#include <cmath>
#include <stdexcept>
#include <string>

using namespace tbb::flow;
using CLBuf = opencl_buffer<cl_int>;
int main() {
  try {
    const int N = 10;

    graph g;
    tbb::flow::opencl_program<> program(std::string("mul.cl"));
    opencl_node<tuple<CLBuf,CLBuf>> gpu_node( g, program.get_kernel("mul"));
    gpu_node.set_range( {{ N }} );

    function_node<int,CLBuf> filler0( g, unlimited, [&g,N]( int i ) {
      CLBuf b(N);
      std::fill( b.begin(), b.end(), i );
      return b;
    } );

    function_node<int,CLBuf> filler1 = filler0;

    function_node<CLBuf> checker( g, serial, []( const CLBuf &b ) {
      for ( cl_int v : b ) {
        int r = int(std::sqrt(v) + .5);
        if ( r*r != v )
        throw std::runtime_error( std::to_string(v) + " is not a square of any integer number" );
      }
    } );

    make_edge( filler0, input_port<0>(gpu_node) );
    make_edge( filler1, input_port<1>(gpu_node) );
    make_edge( output_port<0>(gpu_node), checker );

    for ( int i = 1; i<=1000; ++i ) {
      filler0.try_put( i );
      filler1.try_put( i );
    }
    g.wait_for_all();
  } catch ( std::exception &e ) {
    std::cerr << "Liar!!: " << e.what() << std::endl;
  }
  return 0;
}
