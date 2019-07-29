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

#include <tbb/tbb.h>
#include <string>
#include <iostream>
#include <memory>

static inline void spinWaitForAtLeast(double sec) {
  tbb::tick_count t0 = tbb::tick_count::now();
  while ((tbb::tick_count::now() - t0).seconds() < sec);
}

void warmupTBB() {
  tbb::parallel_for(0, tbb::task_scheduler_init::default_num_threads(), 
  [](int) {
    spinWaitForAtLeast(0.001);
  });
}

struct Message {
  size_t my_seq_no;
  std::string my_string;
  Message(int i) : my_seq_no(i), my_string(std::to_string(i)) { }
};

using MessagePtr = std::shared_ptr<Message>;

void fig_17_22() {
  const int N = 10;
  tbb::flow::graph g;
  tbb::flow::function_node<MessagePtr, MessagePtr> 
  first_node{g, tbb::flow::unlimited, [] (MessagePtr m) {
    m->my_string += " with sequencer";
    return m;
  }};

  using MFNSequencer = tbb::flow::multifunction_node<MessagePtr, std::tuple<MessagePtr>>;
  using MFNPorts = typename MFNSequencer::output_ports_type;

  int seq_i = 0;
  std::vector<MessagePtr> v{(const unsigned)N, MessagePtr{}};

  MFNSequencer sequencer{g, tbb::flow::serial,
  [N, &seq_i, &v](MessagePtr m, MFNPorts& p) { 
    v[m->my_seq_no] = m;
    while (seq_i < N && v[seq_i].use_count()) {
      std::get<0>(p).try_put(v[seq_i++]);
    }
  }};

  tbb::flow::function_node<MessagePtr, int> 
  last_node{g, tbb::flow::serial, [] (MessagePtr m) {
      std::cout << m->my_string << std::endl;
      return 0;
  }};
  tbb::flow::make_edge(first_node, sequencer);
  tbb::flow::make_edge(sequencer, last_node);

  for (int i = 0; i < N; ++i) 
    first_node.try_put(std::make_shared<Message>(i));
  g.wait_for_all();
}

void fig_17_22_no_sequencer() {
  const int N = 10;
  tbb::flow::graph g;
  tbb::flow::function_node<MessagePtr, MessagePtr> 
  first_node{g, tbb::flow::unlimited, [] (MessagePtr m) {
      m->my_string += " no sequencer";
      return m;
  }};
  tbb::flow::function_node<MessagePtr, int> 
  last_node{g, tbb::flow::serial, [] (MessagePtr m) {
      std::cout << m->my_string << std::endl;
      return 0;
  }};
  tbb::flow::make_edge(first_node, last_node);

  for (int i = 0; i < N; ++i) 
    first_node.try_put(std::make_shared<Message>(i));
  g.wait_for_all();
}


int main() {
  warmupTBB();
  tbb::tick_count t0 = tbb::tick_count::now();
  fig_17_22_no_sequencer();
  double time_wo_sequencer = (tbb::tick_count::now()-t0).seconds();
  std::cout << std::endl;

  warmupTBB();
  t0 = tbb::tick_count::now();
  fig_17_22();
  double time_with_sequencer = (tbb::tick_count::now()-t0).seconds();
  std::cout << std::endl;

  std::cout << "Time with no sequencer == " << time_wo_sequencer << std::endl
            << "Time with sequencer == " << time_with_sequencer << std::endl;
  return 0;
}

