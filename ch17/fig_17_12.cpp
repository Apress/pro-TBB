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

void spinWaitForAtLeast(double sec) {
  tbb::tick_count t0 = tbb::tick_count::now();
  while ((tbb::tick_count::now() - t0).seconds() < sec);
}

class BigObject {
  const int id;
  /* And a big amount of other data */
public:
  BigObject() : id(-1) { } 
  /* We simulate a big copy time with spins */
  BigObject(int i) : id(i) {spinWaitForAtLeast(0.001);}
  BigObject(const BigObject& b) : id(b.id) {spinWaitForAtLeast(0.001);}
  int get_id() const {return id;}
};

void fig_17_12() {
  tbb::flow::graph g;

  tbb::flow::function_node<std::shared_ptr<BigObject>, int> 
  n(g, tbb::flow::serial,
    [](std::shared_ptr<BigObject> b) -> int {
      int id = b->get_id();
      spinWaitForAtLeast(0.01); 
      return id; 
    }
  );
   
  for (int i = 0; i < 100; ++i) {
    n.try_put(std::make_shared<BigObject>(i));
  }
  g.wait_for_all();
}

void bigObjectCopy() {
  tbb::flow::graph g;

  tbb::flow::function_node<BigObject, int> 
  n(g, tbb::flow::serial,
    [](const BigObject& b) -> int {
      int id = b.get_id();
      spinWaitForAtLeast(0.01); 
      return id; 
    }
  );
   
  for (int i = 0; i < 100; ++i) {
    n.try_put(BigObject{i});
  }
  g.wait_for_all();
}

void warmupTBB() {
  tbb::parallel_for(0, tbb::task_scheduler_init::default_num_threads(), 
    [](int) {
      spinWaitForAtLeast(0.001);
    }
  );
}

int main(int argc, char *argv[]) {
  warmupTBB();
  tbb::tick_count t0 = tbb::tick_count::now();
  fig_17_12();
  tbb::tick_count t1 = tbb::tick_count::now();
  bigObjectCopy();
  tbb::tick_count t2 = tbb::tick_count::now();
  std::cout << "Time with copies == " << (t2-t1).seconds() << std::endl
            << "Time with shared_ptr == " << (t1-t0).seconds() << std::endl;
  return 0;
}

