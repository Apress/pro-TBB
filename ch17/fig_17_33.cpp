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
#include <memory>
#include <tbb/tbb.h>

using tokenn_t = int;
const int A_LARGE_NUMBER = 100;

void spinWaitForAtLeast(double sec) {
  tbb::tick_count t0 = tbb::tick_count::now();
  while ((tbb::tick_count::now() - t0).seconds() < sec);
}

void warmupTBB() {
  tbb::parallel_for(0, tbb::task_scheduler_init::default_num_threads(), 
  [](int) {
    spinWaitForAtLeast(0.001);
  });
}

tbb::atomic<int> bigObjectCount;
int maxCount = 0;

class BigObject {
   int id;
   /* And a big amount of other data */
public:
   BigObject() : id(-1) { } 
   BigObject(int i) : id(i) { 
     int cnt = bigObjectCount.fetch_and_increment() + 1;
     if (cnt > maxCount) 
       maxCount = cnt;
   }
   BigObject(const BigObject& b) : id(b.id) { }
   virtual ~BigObject() {
     bigObjectCount.fetch_and_decrement();
   }
   int getId() const {return id;}
   int mergeIds(int id0, int id1) {return id = id1*100 + id0;}
};

using BigObjectPtr = std::shared_ptr<BigObject>;

using CompositeType = 
  tbb::flow::composite_node<std::tuple<BigObjectPtr, BigObjectPtr>,
                            std::tuple<BigObjectPtr>>;

class MergeNode : public CompositeType {
  tbb::flow::buffer_node<tokenn_t> tokenBuffer;
  tbb::flow::join_node<std::tuple<BigObjectPtr, BigObjectPtr, tokenn_t>, 
                       tbb::flow::reserving> join;
  using MFNode = 
    tbb::flow::multifunction_node<std::tuple<BigObjectPtr, BigObjectPtr, tokenn_t>,
                                  std::tuple<BigObjectPtr, tokenn_t>>;
  MFNode combine;

public:
  MergeNode(tbb::flow::graph& g) :
    CompositeType{g},
    tokenBuffer{g},
    join{g},
    combine{g, tbb::flow::unlimited,
      [] (const MFNode::input_type& in, MFNode::output_ports_type& p) {
        BigObjectPtr b0 = std::get<0>(in); 
        BigObjectPtr b1 = std::get<1>(in); 
        tokenn_t t = std::get<2>(in);
        spinWaitForAtLeast(0.0001);      
        b0->mergeIds(b0->getId(), b1->getId());      
        std::get<0>(p).try_put(b0);
        std::get<1>(p).try_put(t);
      }}
  {
    // make the edges
    tbb::flow::make_edge(tokenBuffer, tbb::flow::input_port<2>(join));
    tbb::flow::make_edge(join, combine);
    tbb::flow::make_edge(tbb::flow::output_port<1>(combine), tokenBuffer);

    // set the input and output ports
    CompositeType::set_external_ports(
      CompositeType::input_ports_type(
        tbb::flow::input_port<0>(join),
        tbb::flow::input_port<1>(join)
      ),
      CompositeType::output_ports_type(
        tbb::flow::output_port<0>(combine)
      )
    );

    for (tokenn_t i = 0; i < 3; ++i)
      tokenBuffer.try_put(i);
  }
};

void fig_17_33() {
  tbb::flow::graph g;

  int src1_count = 0;
  tbb::flow::source_node<BigObjectPtr> source1{g, 
  [&] (BigObjectPtr& m) -> bool {
    if (src1_count < A_LARGE_NUMBER) {
      m = std::make_shared<BigObject>(src1_count++);
      return true;
    }
    return false;
  }, false};

  int src2_count = 0;
  tbb::flow::source_node<BigObjectPtr> source2{g, 
  [&] (BigObjectPtr& m) -> bool {
    if (src2_count < A_LARGE_NUMBER) {
      m = std::make_shared<BigObject>(src2_count++);
      return true;
    }
    return false;
  }, false};

  MergeNode merge{g};

  tbb::flow::function_node<BigObjectPtr> output{g, 
    tbb::flow::serial,
    [] (BigObjectPtr b) {
      std::cout << "Received id == " << b->getId() 
                << " in final node" << std::endl;
    }
  };

  tbb::flow::make_edge(source1, tbb::flow::input_port<0>(merge));
  tbb::flow::make_edge(source2, tbb::flow::input_port<1>(merge));
  tbb::flow::make_edge(merge, output);

  bigObjectCount = 0;
  maxCount = 0;
  source1.activate();
  source2.activate();
  g.wait_for_all();
  std::cout << "maxCount == " << maxCount << std::endl;
}

int main(int argc, char *argv[]) {
  warmupTBB();
  tbb::tick_count t0 = tbb::tick_count::now();
  fig_17_33();
  tbb::tick_count t1 = tbb::tick_count::now();
  std::cout << "Total time == " << (t1-t0).seconds() << std::endl;
  return 0;
}

