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

#include <iostream>
#include <tbb/tbb.h>

static inline void spinWaitForAtLeast(double sec) {
  tbb::tick_count t0 = tbb::tick_count::now();
  while ((tbb::tick_count::now() - t0).seconds() < sec);
}

double fig_17_2(int num_trials, int N, double per_node_time) {
  tbb::tick_count t0, t1;
  using node_t = tbb::flow::multifunction_node<int, std::tuple<int>>;
  tbb::flow::graph g;
  node_t n{g, tbb::flow::unlimited, 
    [N, per_node_time](int i, node_t::output_ports_type& p) -> void {
      spinWaitForAtLeast(per_node_time); 
      if (i+1 < N) {
        std::get<0>(p).try_put(i+1);
      }
    }
  };
  tbb::flow::make_edge(tbb::flow::output_port<0>(n), n);

  for (int t = -1; t < num_trials; ++t) { 
    if (!t) t0 = tbb::tick_count::now();
    n.try_put(0);
    g.wait_for_all();
  }
  t1 = tbb::tick_count::now();
  return (t1-t0).seconds()/num_trials;
}

int main() {
  const int P = tbb::task_scheduler_init::default_num_threads();
  const int NUM_TRIALS = 2;
  const int H = 16;
  const int N = (1<<H) - 1;
  const int PER_NODE_TIMES = 4;
  double per_node_time[PER_NODE_TIMES] = { 1e-7, 1e-6, 1e-5, 1e-4 };

  std::cout << "The system has " << P << " threads" << std::endl 
            << "time in seconds for the serial flow graph loop:" 
            << std::endl << std::endl
            << "version, 100ns, 1us, 10us, 100us" << std::endl 
            << "FG loop";

  for (double tpn : per_node_time) {
    double serial_fg_time = fig_17_2(NUM_TRIALS, N, tpn);
    std::cout << ", " << serial_fg_time;
  }
  std::cout << std::endl;
  return 0;
}

