#define TBB_PREVIEW_FLOW_GRAPH_NODES 1
#define TBB_PREVIEW_FLOW_GRAPH_FEATURES 1
//#include "tbb/flow_graph.h"
#include "tbb/flow_graph_opencl_node.h"

#include <iostream>

int main() {
  using namespace tbb::flow;

  const tbb::flow::opencl_device_list &devices =
  // if in flow_graph_opencl_node.h we add this line at the end:
  // using interface10::opencl_info::available_devices;
    //tbb::flow::available_devices();
    //else:
    tbb::flow::interface10::opencl_info::available_devices();
  opencl_device d=*devices.cbegin();
  std::cout << "Platform: " << d.platform_name() << '\n';
  std::cout << "Platform profile: " << d.platform_profile() << '\n';
  std::cout << "Platform version: " << d.platform_version() << '\n';
  std::cout << "Platform vendor: " << d.platform_vendor() << '\n';
  std::cout << "Platform extensions: " << d.platform_extensions() << '\n';
  std::cout << '\n';
  for ( opencl_device d : devices ) {
    std::cout << "Device: " << d.name() << '\n';
    std::cout << "  Major version: " << d.major_version();
    std::cout << ";  Minor version: " << d.minor_version() << '\n';
    std::cout << "  Device type: ";
    switch (d.type()) {
      case CL_DEVICE_TYPE_GPU:
      std::cout << "GPU" << '\n';
      break;
      case CL_DEVICE_TYPE_CPU:
      std::cout << "CPU" << '\n';
      break;
      default:
      std::cout << "Unknown" << '\n';
    }
  }

  tbb::flow::opencl_device_list::const_iterator it = std::find_if(
    devices.cbegin(), devices.cend(),
    [](const tbb::flow::opencl_device &d) {
      cl_device_type type;
      d.info(CL_DEVICE_TYPE, type);
      return CL_DEVICE_TYPE_GPU == type;
    });
    opencl_device od=*it;
    std::cout << od.version() << '\n';

    return 0;
  }
