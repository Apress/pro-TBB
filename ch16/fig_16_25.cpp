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

#include <fstream>
#include <iostream>
#include <tbb/tbb.h>
#include <thread>

class SourceFilter : public tbb::filter {
public:
  SourceFilter(size_t n);
  void *operator() (void *) override;
private:
  size_t numItems;
};

class MultiplyFilter : public tbb::filter {
public:
  MultiplyFilter();
  void *operator() (void *v) override;
};

thread_local std::ofstream output;

class BadWriteFilter : public tbb::filter {
public:
  BadWriteFilter() 
    : tbb::filter(tbb::filter::serial_in_order), issued_error(false) { }
  void *operator() (void *v) override {
    if (output.is_open()) {
       output << reinterpret_cast<size_t>(v) << std::endl;    
    } else if (!issued_error) {
      std::cerr << "Error!" << std::endl;
      issued_error = true; 
    }
    return NULL;
  }
private:
  bool issued_error;
};

void fig_16_25() {
  output.open("output.txt", std::ofstream::out);

  SourceFilter sf(100);
  MultiplyFilter mf;
  BadWriteFilter wf;

  tbb::pipeline p;
  p.add_filter(sf);
  p.add_filter(mf);
  p.add_filter(wf);
  p.run(tbb::task_scheduler_init::default_num_threads());
  std::cout << "Done." << std::endl;
}

void doWork(double sec) {
  tbb::tick_count t0 = tbb::tick_count::now();
  while ((tbb::tick_count::now() - t0).seconds() <= sec);
}

SourceFilter::SourceFilter(size_t n) 
  : tbb::filter(tbb::filter::serial_in_order), numItems(n) { }

void *
SourceFilter::operator()(void *) {
  if (numItems > 0) {
    doWork(0.0001);
    return reinterpret_cast<void *>(numItems--);
  } else {
    return NULL;
  }
}

MultiplyFilter::MultiplyFilter() 
    : tbb::filter(tbb::filter::parallel) { }

void *
MultiplyFilter::operator()(void *v) {
  doWork(0.0001);
  return reinterpret_cast<void *>(reinterpret_cast<size_t>(v)*2);
}

int main() {
  fig_16_25();
  return 0;
}

