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
#include <map>
#include <set>
#include <thread>
#include <vector>
#include <tbb/tbb.h>

const int P = tbb::task_scheduler_init::default_num_threads();
thread_local int my_tid = -1;
void doWork(const std::string& name, double seconds);

void fig_11_8() {
  int N = 10*P;
  tbb::task_arena a;

  std::thread t([=]() {
    tbb::parallel_for(0, N, [](int) { doWork("std::thread pfor", 0.01); });
  });

  a.execute([=]() {
    tbb::parallel_for(0, N, [](int) { doWork("arena pfor", 0.01); });
  });

  tbb::parallel_for(0, N, [](int) { doWork("main pfor", 0.01); });
  t.join();
}

std::vector<std::set<std::string>> tid_regions(3*P);
tbb::atomic<int> next_tid;

void noteParticipation(const std::string& name) {
  if (my_tid == -1) {
    my_tid = next_tid.fetch_and_increment();
  }
  tid_regions[my_tid].insert(name);
}

void doWork(const std::string& name, double seconds) {
  noteParticipation(name);
  tbb::tick_count t0 = tbb::tick_count::now();
  while ((tbb::tick_count::now() - t0).seconds() < seconds);
}

void dump_participation() {
  int end = next_tid;
  std::map<std::string, int> m;
  for (int i = 0; i < end; ++i) {
    for (auto n : tid_regions[i]) {
      m[n] += 1;
    }
  }
  std::cout << "There are " << tbb::task_scheduler_init::default_num_threads() << " logical cores." << std::endl;
  for (auto& kv : m) {
    std::cout << kv.second << " threads participated in " << kv.first << std::endl;
  }
}

int main() {
  next_tid = 0;
  fig_11_8();
  dump_participation();
  return 0;
}

