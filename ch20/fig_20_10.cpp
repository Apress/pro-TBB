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

#define TBB_PREVIEW_LOCAL_OBSERVER 1
#include <hwloc.h>
#include <cstdio>
#include <iostream>
#include <cstring>
#include <vector>
#include <thread>
#include <assert.h>
#include <tbb/tbb.h>

std::vector<double> times;

class PinningObserver : public tbb::task_scheduler_observer {
  hwloc_topology_t topo;
  hwloc_obj_t numa_node;
  int numa_id;
  int num_nodes;
  tbb::atomic<int> thds_per_node;
  tbb::atomic<int> masters_that_entered;
  tbb::atomic<int> workers_that_entered;
  tbb::atomic<int> threads_pinned;
public:
  PinningObserver(tbb::task_arena& arena, hwloc_topology_t& _topo,
                  int _numa_id, int _thds_per_node)
                  : task_scheduler_observer{arena}, topo{_topo},
                    numa_id{_numa_id}, thds_per_node{_thds_per_node}
  {
    num_nodes = hwloc_get_nbobjs_by_type(topo,
                                      HWLOC_OBJ_NUMANODE);
    numa_node = hwloc_get_obj_by_type(topo,
                                  HWLOC_OBJ_NUMANODE,numa_id);
    masters_that_entered = 0;
    workers_that_entered = 0;
    threads_pinned = 0;
    observe(true);
  }
  virtual ~PinningObserver() {
    int nid =numa_id;
    int nmt = masters_that_entered, nwt = workers_that_entered;
    int np = threads_pinned;
    std::printf("Node %d, numMasters %d, numWorkers %d, numPinned %d \n", nid,nmt,nwt,np);
  }
  void on_scheduler_entry(bool is_worker) {
    if (is_worker) ++workers_that_entered;
    else ++masters_that_entered;
    //std::printf("Thread %d enters arena %d\n", std::this_thread::get_id(),numa_id);
    if(--thds_per_node > 0){
      int err=hwloc_set_cpubind(topo, numa_node->cpuset,
            HWLOC_CPUBIND_THREAD);
      std::cout << "Error setting CPU bind on this platform\n";
      //std::printf("Pinned thread %d to NUMA node %d\n", std::this_thread::get_id(), numa_id);
      threads_pinned++;
    }
  }
};

void alloc_mem_per_node(hwloc_topology_t topo,
                        double **data,
                        long size){
  int num_nodes = hwloc_get_nbobjs_by_type(topo, HWLOC_OBJ_NUMANODE);
  for(int i=0 ; i< num_nodes ; i++){ //for each NUMA node
    hwloc_obj_t numa_node = hwloc_get_obj_by_type(topo,
                                              HWLOC_OBJ_NUMANODE, i);

    if (numa_node) {
      data[i] = (double *) hwloc_alloc_membind(topo,
                          size*sizeof(double), numa_node->nodeset,
                          HWLOC_MEMBIND_BIND, HWLOC_MEMBIND_BYNODESET);
    }
  }
}

void alloc_thr_per_node(hwloc_topology_t topo, double** data,
                        size_t lsize, int thds_per_node){
  float alpha = 0.5;
  int num_nodes = hwloc_get_nbobjs_by_type(topo, HWLOC_OBJ_NUMANODE);
  //int nPUs = hwloc_get_nbobjs_by_type(topo, HWLOC_OBJ_PU);
  std::vector<std::thread> vth;
  for(int i = 0; i < num_nodes; i++){
    vth.push_back(std::thread{
      [=,&topo](){
        hwloc_obj_t numa_node = hwloc_get_obj_by_type(topo,
                                            HWLOC_OBJ_NUMANODE,i);
        int err = hwloc_set_cpubind(topo, numa_node->cpuset,
                                    HWLOC_CPUBIND_THREAD);
        std::cout << "Error setting CPU bind on this platform\n";
        double *A = data[i];
        double *B = data[i] + lsize;
        double *C = data[i] + 2*lsize;

        for(size_t j = 0; j < lsize; j++){
          A[j] = j; B[j] = j;
        }
        //task_arena numa_arena(thds_per_node*num_nodes);
        tbb::task_arena numa_arena{thds_per_node};
        PinningObserver p{numa_arena, topo, i, thds_per_node};
        auto t = tbb::tick_count::now();
        numa_arena.execute([&](){
          tbb::parallel_for(tbb::blocked_range<size_t>{0, lsize},
            [&](const tbb::blocked_range<size_t>& r){
              for (size_t i = r.begin(); i < r.end(); ++i)
                C[i] = A[i] + alpha * B[i];
            });
        });
        double ts = (tbb::tick_count::now() - t).seconds();
        times[i] = ts;
      }
    });
  }
  for (auto& th: vth) th.join();
}

int main(int argc, char** argv)
{
  int thds_per_node = 8;
  size_t vsize = 100000000;

  hwloc_topology_t topo;
  hwloc_topology_init(&topo);
  hwloc_topology_load(topo);

  //* Print the number of NUMA nodes.
  int num_nodes = hwloc_get_nbobjs_by_type(topo, HWLOC_OBJ_NUMANODE);
  std::cout << "There are " << num_nodes << " NUMA node(s)\n";

  double **data = new double*[num_nodes];
  times = std::vector<double>(num_nodes);
  //* Allocate some memory on each NUMA node
  long doubles_per_node = vsize*3/num_nodes;
  alloc_mem_per_node(topo, data, doubles_per_node);

  //* One master thread per NUMA node
  tbb::task_scheduler_init init{(thds_per_node-1)*num_nodes};
  auto t = tbb::tick_count::now();
  alloc_thr_per_node(topo, data, vsize/num_nodes, thds_per_node);
  double ts = (tbb::tick_count::now() - t).seconds();
  std::cout << "Total time (incl. init A and B): " << ts << " seconds -> "
  //vsize * 5 (2 stores --init A & B-- + 1str + 2ld triad )
            << vsize*5*8/ts/1000000.0 << " MB/s \n";

  double time = times[0];
  for(int i = 1; i < num_nodes; i++){
    if(time < times[i]) time = times[i];
  }
  std::cout << "Slower thread time: " << time << " seconds -> "
            << vsize*3*8/time/1000000.0 << " MB/s \n";
  //* Free the allocated data and topology
  for(int i = 0; i < num_nodes; i++){
    hwloc_free(topo, data[i], doubles_per_node);
  }
  hwloc_topology_destroy(topo);
  delete [] data;
  return 0;
}
