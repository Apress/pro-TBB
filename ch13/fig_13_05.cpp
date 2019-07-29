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
#define TBB_PREVIEW_LOCAL_OBSERVER 1 
#include <tbb/tbb.h>

const int numTasks = 8;
tbb::task::affinity_id a[numTasks];
void doWork();
void printExclaim(int id, int slot);

class MyTask : public tbb::task {
  int taskId;
  bool doMakeNotes;
    
public:
  MyTask(int id, bool do_make_notes = true) 
    : taskId(id), doMakeNotes(do_make_notes) { }

  void note_affinity(tbb::task::affinity_id id) override {
    if (doMakeNotes) a[taskId] = id;
  }

  tbb::task *execute() override {
    auto slot_number = tbb::this_task_arena::current_thread_index();
    printExclaim(taskId, slot_number); 
    doWork();
    return NULL;
  }
};


void executeTaskTree(const std::string& name, bool note_affinity, 
                     bool set_affinity) {
  std::cout << name << std::endl
            << "id:slot" << std::endl;
  tbb::task *root = new( tbb::task::allocate_root() ) tbb::empty_task;
  root->set_ref_count(numTasks+1);
  for (int i = 0; i < numTasks; ++i) {
    tbb::task *t = new ( root->allocate_child() ) MyTask(i,note_affinity); 
    if (set_affinity)
        t->set_affinity(a[2]);
    tbb::task::spawn(*t);
  }
  root->wait_for_all();
}

void fig_13_5() {
  std::fill(a, a+numTasks, 0);  
  executeTaskTree("note_affinity", true, false);
  executeTaskTree("with set_affinity to a[2]", false, true);
}

void doWork() {
  tbb::tick_count t0 = tbb::tick_count::now();
  while ((tbb::tick_count::now() - t0).seconds() < 0.01);
}

void printExclaim(int id, int slot) {
  std::string out = std::to_string(id)
                    + ":" + std::to_string(slot) + "\n";
  std::cout << out;
}

int main() {
  fig_13_5();
  return 0;
}

