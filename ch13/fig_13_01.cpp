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

/*
 Figure 13-1: Using a task_scheduler_observer to pin threads to cores on a Linux platform.
 Therefore - this is Linux ONLY code (the scheduler calls are Linux specific)
*/

#include <iostream>
#include <sched.h>
#define TBB_PREVIEW_LOCAL_OBSERVER 1 
#include <tbb/tbb.h>

thread_local int my_cpu = -1;
void doWork();

class pinning_observer : public tbb::task_scheduler_observer {
public:
    pinning_observer() { observe(true); }

    void on_scheduler_entry( bool is_worker ) {
        cpu_set_t *mask;
        auto number_of_slots = tbb::this_task_arena::max_concurrency();
        mask = CPU_ALLOC(number_of_slots);
        auto mask_size = CPU_ALLOC_SIZE(number_of_slots);

        auto slot_number = tbb::this_task_arena::current_thread_index();
        CPU_ZERO_S(mask_size, mask); 
        CPU_SET_S(slot_number, mask_size, mask); 
        if (sched_setaffinity( 0, mask_size, mask )) {
           std::cout << "Error in sched_setaffinity" << std::endl;
        }
        // so we can see if it worked:
        my_cpu = sched_getcpu();
    }
};

void fig_13_1() {
  const int N = 100;

  std::cout << "Without pinning:" << std::endl;
  tbb::parallel_for(0, N, [](int) {doWork();});

  std::cout << std::endl 
            << "With pinning:" << std::endl;
  pinning_observer p;
  tbb::parallel_for(0, N, [](int) {doWork();});
  std::cout << std::endl; 
}

void doWork() {
  int current_cpu = sched_getcpu();
  int slot_number = tbb::this_task_arena::current_thread_index();
  if (current_cpu == slot_number) {
     std::cout << "Y ";
  } else {
     std::cout << "N ";
  }
  tbb::tick_count t0 = tbb::tick_count::now();
  while ((tbb::tick_count::now() - t0).seconds() < 0.001);
}

int main() {
    fig_13_1();
    return 0;
}

