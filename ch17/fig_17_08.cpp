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

#include <memory>
#include <tbb/tbb.h>
#include <iostream>

static inline void spinWaitForAtLeast(double sec) {
  if (sec == 0) return;
  tbb::tick_count t0 = tbb::tick_count::now();
  while ((tbb::tick_count::now() - t0).seconds() < sec);
}

void warmupTBB() {
  tbb::parallel_for(0, tbb::task_scheduler_init::default_num_threads(), 
    [](int) {
      spinWaitForAtLeast(0.001);
    }
  );
}

template <typename Policy>
double build_and_run_double_chain(int N, double per_body_time) {
  using root_t = tbb::flow::multifunction_node<int, std::tuple<int,int>, Policy>;
  using root_ports_t = typename root_t::output_ports_type;
  using node_t = tbb::flow::multifunction_node<int, std::tuple<int>, Policy>;
  using node_ports_t = typename node_t::output_ports_type;
  using node_ptr = std::shared_ptr<node_t>;
  std::vector<node_ptr> node_list;

  tbb::flow::graph g;
  root_t root(g, tbb::flow::unlimited,
    [](int i, root_ports_t& p) -> void {
      std::get<0>(p).try_put(i);
      std::get<1>(p).try_put(i);
    }
  );

  node_ptr head1_ptr = std::make_shared<node_t>(g, tbb::flow::unlimited, 
    [=](int i, node_ports_t& p) -> void {
      spinWaitForAtLeast(per_body_time);
      std::get<0>(p).try_put(i);
    }
  );
  node_ptr head2_ptr = std::make_shared<node_t>(g, tbb::flow::unlimited, 
    [=](int i, node_ports_t& p) -> void {
      spinWaitForAtLeast(per_body_time);
      std::get<0>(p).try_put(i);
    }
  );
  tbb::flow::make_edge(tbb::flow::output_port<0>(root), *head1_ptr);
  tbb::flow::make_edge(tbb::flow::output_port<1>(root), *head2_ptr);

  node_ptr last_node = head1_ptr;
  for (int i = 0; i < N-1; ++i) {
    node_ptr new_node = std::make_shared<node_t>(g, tbb::flow::unlimited, 
      [=](int i, node_ports_t& p) -> void {
        spinWaitForAtLeast(per_body_time);
        std::get<0>(p).try_put(i);
      }
    );
    tbb::flow::make_edge(*last_node, *new_node);
    last_node = new_node;
    node_list.push_back(new_node);
  } 

  last_node = head2_ptr;
  for (int i = 0; i < N-1; ++i) {
    node_ptr new_node = std::make_shared<node_t>(g, tbb::flow::unlimited, 
      [=](int i, node_ports_t& p) -> void {
        spinWaitForAtLeast(per_body_time);
        std::get<0>(p).try_put(i);
      }
    );
    tbb::flow::make_edge(*last_node, *new_node);
    last_node = new_node;
    node_list.push_back(new_node);
  } 

   warmupTBB();
   tbb::tick_count t0 = tbb::tick_count::now();
   root.try_put(0);
   g.wait_for_all();
   tbb::tick_count t1 = tbb::tick_count::now();
   return (t1-t0).seconds();
}

template<typename Policy>
double build_and_run_chain(int N, double per_body_time) {
  using node_t = tbb::flow::multifunction_node<int, std::tuple<int>, Policy>;
  using node_ports_t = typename node_t::output_ports_type;
  using node_ptr = std::shared_ptr<node_t>;
  std::vector<node_ptr> node_list;

  node_ptr last_node;
  node_ptr first_node;

  tbb::flow::graph g;
  for (int i = 0; i < N; ++i) {
    node_ptr new_node = std::make_shared<node_t>(g, tbb::flow::unlimited, 
      [=](int i, node_ports_t& p) -> int {
        spinWaitForAtLeast(per_body_time);
        return std::get<0>(p).try_put(i);
      }
    );
    if (!first_node) first_node = new_node;
    if (last_node) tbb::flow::make_edge(*last_node, *new_node);
    last_node = new_node;
    node_list.push_back(new_node);
  } 

  warmupTBB();
  tbb::tick_count t0 = tbb::tick_count::now();
  first_node->try_put(0);
  g.wait_for_all();
  return (tbb::tick_count::now()-t0).seconds();
}

int main() {
   std::vector<double> spin_times = { 0, 1e-9, 1e-8, 1e-7, 1e-6, 1e-5, 1e-4, 1e-3 }; 

   std::cout << "Speedup: default / lightweight:" << std::endl << std::endl
             << "chain type";

   for ( auto t : spin_times ) {
     std::cout << ", " << t;
   }
   std::cout << std::endl;
             
   std::cout << "One chain";
   for ( auto t : spin_times ) {
     double normal_time = build_and_run_chain<tbb::flow::queueing>(1000, t);
     double lw_time = build_and_run_chain<tbb::flow::lightweight>(1000, t);
     std::cout << ", " << normal_time/lw_time;
   }
   std::cout << std::endl;

   std::cout << "Two chain";
   for ( auto t : spin_times ) {
     double normal_time = build_and_run_double_chain<tbb::flow::queueing>(1000, t);
     double lw_time = build_and_run_double_chain<tbb::flow::lightweight>(1000, t);
     std::cout << ", " << normal_time/lw_time;
  }
  std::cout << std::endl;
  return 0;
}

