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

#include <hwloc.h>
#include <iostream>
#include <vector>
#include <thread>
#include <sstream>
#include <tbb/tbb.h>

#ifdef __APPLE__
// From https://stackoverflow.com/questions/33745364/sched-getcpu-equivalent-for-os-x
#include <cpuid.h>
#define CPUID(INFO, LEAF, SUBLEAF) __cpuid_count(LEAF, SUBLEAF, INFO[0], INFO[1], INFO[2], INFO[3])
#define GETCPU(CPU) {                                   \
        uint32_t CPUInfo[4];                            \
        CPUID(CPUInfo, 1, 0);                           \
        /* CPUInfo[1] is EBX, bits 24-31 are APIC ID */ \
        if ( (CPUInfo[3] & (1 << 9)) == 0) {            \
          CPU = -1;  /* no APIC on chip */              \
        }                                               \
        else {                                          \
          CPU = (unsigned)CPUInfo[1] >> 24;             \
        }                                               \
        if (CPU < 0) CPU = 0;                           \
      }
#else

#endif
int cpuid(){
  int id;
#ifdef __APPLE__
  GETCPU(id);
#else
  id = sched_getcpu();
#endif
  return id;
}

void alloc_mem_per_node(hwloc_topology_t topo,
                        double **data,
                        long size){
  int num_nodes = hwloc_get_nbobjs_by_type(topo, HWLOC_OBJ_NUMANODE);
  for(int i = 0; i < num_nodes; i++) { //for each NUMA node
    hwloc_obj_t numa_node = hwloc_get_obj_by_type(topo,
                                              HWLOC_OBJ_NUMANODE, i);
    if (numa_node) {
      char *s;
      hwloc_bitmap_asprintf(&s, numa_node->cpuset);
      //hwloc_obj_attr_snprintf(s, sizeof(s), numa_node, "\n", 3);
      std::cout << "NUMA node " << i << " has cpu bitmask: " << s << '\n';
      free(s);
      hwloc_bitmap_asprintf(&s, numa_node->nodeset);
      std::cout << "Allocate data on node " << i
                << " with node bitmask " << s << '\n';
      free(s);

      data[i] = (double *) hwloc_alloc_membind(topo,
        size*sizeof(double), numa_node->nodeset,
        HWLOC_MEMBIND_BIND, HWLOC_MEMBIND_BYNODESET);
    }
  }
}

using sout_t = tbb::enumerable_thread_specific<std::stringstream>;
void alloc_thr_per_node(hwloc_topology_t topo, sout_t& sout){
  int num_nodes = hwloc_get_nbobjs_by_type(topo, HWLOC_OBJ_NUMANODE);
  std::vector<std::thread> vth;
  for(int i = 0; i < num_nodes; i++){
    vth.push_back(std::thread{[i, num_nodes, &topo, &sout]()
    {
      int err;
      sout.local() << "I'm masterThread: " << i << " out of "
                   << num_nodes << '\n';
      sout.local() << "Before: Thread: " << i
                   << " with tid " << std::this_thread::get_id()
                   << " on core " << cpuid() << '\n';

      hwloc_obj_t numa_node = hwloc_get_obj_by_type(topo,
                                              HWLOC_OBJ_NUMANODE,i);
      if (numa_node) {
        err = hwloc_set_cpubind(topo, numa_node->cpuset,
                                HWLOC_CPUBIND_THREAD);
        if(err){
          sout.local() << "Error setting CPU bind on this platform\n";
        };
        sout.local() << "After: Thread: " << i
                     << " with tid " << std::this_thread::get_id()
                     << " on core " << cpuid() << '\n';
      }
    }});
  }
  for(auto& th: vth) th.join();
}

int main(void)
{
  hwloc_topology_t topo;
  hwloc_topology_init(&topo);
  hwloc_topology_load(topo);

  //* Print the number of NUMA nodes.
  int num_nodes = hwloc_get_nbobjs_by_type(topo, HWLOC_OBJ_NUMANODE);
  std::cout << "There are " << num_nodes << " NUMA node(s)\n";

  double ** data = new double*[num_nodes];
  //* Allocate some memory on each NUMA node
  long size = 1024*1024;
  alloc_mem_per_node(topo, data, size);

  sout_t sout;
  //* One master thread per NUMA node
  alloc_thr_per_node(topo, sout);

  for (auto& s : sout) {
    std::cout << s.str();
  }
  //* Free the allocated data and topology
  for(int i = 0; i < num_nodes; i++){
    hwloc_free(topo, data[i], size);
  }
  hwloc_topology_destroy(topo);
  delete [] data;
  return 0;
}
