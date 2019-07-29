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
#include <string>

#include <tbb/tbb.h>

void doWork(double sec);

class Spinner : public tbb::task {
public:
  Spinner(const char *m, int id, double len); 
  tbb::task *execute() override;

private:
  std::string msg;
  int messageId;
  double length;
};

void enqueueTask(const char *msg, 
                 int id, 
                 double len,
                 tbb::priority_t priority=tbb::priority_normal) {
  tbb::task::enqueue(*new( tbb::task::allocate_root()) 
    Spinner( msg, id, len ),
    priority);
}

void fig_14_02() {
  int P = tbb::task_scheduler_init::default_num_threads();

  enqueueTask("N", 0, 0.5);
  doWork(0.01);

  for (int i = 0; i < P; ++i) {
    enqueueTask("L", i, 0.01, tbb::priority_low);
    enqueueTask("N", i+1, 0.01, tbb::priority_normal);
    enqueueTask("H", i, 0.01, tbb::priority_high);
  }
  doWork(1.0);
}

void doWork(double sec) {
  tbb::tick_count t0 = tbb::tick_count::now();
  while ((tbb::tick_count::now() - t0).seconds() < sec);
}

#include <sstream>

tbb::enumerable_thread_specific<std::stringstream> sout;

void printTrace() {
  for (auto& s : sout) {
    std::cout << s.str() << std::endl;
  }
}

Spinner::Spinner(const char *m, int i, double len) : 
  msg(m), messageId(i), length(len) {
}

tbb::task *
Spinner::execute() {
  sout.local() << msg.c_str() << ":" << messageId << " ";
  doWork(length); 
  return NULL;
}

int main() {
  fig_14_02();
  printTrace();
  return 0;
}

