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

class WorkItem {
public:
  WorkItem() {}
  WorkItem(int p) : priority(p) { }

  bool operator<(const WorkItem& b) const {
    if (priority < b.priority) 
      return true;
    else
      return false;
  }

  void doWork();

private:
  int priority;
};

void fig_14_4() {
  tbb::concurrent_priority_queue<WorkItem> q;
  tbb::task_group g;

  const std::string prefix = "w";
  for (int i = 0; i < 16; ++i) {
    q.push(WorkItem(i));
    g.run([&q]() { 
      WorkItem w; 
      if (q.try_pop(w)) 
        w.doWork();
    });
  }
  g.wait();
}

void 
WorkItem::doWork() {
  std::string w = "WorkItem: " + std::to_string(priority) + "\n";
  std::cout << w;
  tbb::tick_count t0 = tbb::tick_count::now();
  while ((tbb::tick_count::now() - t0).seconds() < 0.01);
}

int main() {
  fig_14_4();
  return 0;
}

