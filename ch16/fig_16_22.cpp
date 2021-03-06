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
#include <vector>

void doWork(double sec);

class SourceFilter : public tbb::filter {
  std::vector<int>& data;
  double usec;
  tbb::atomic<int> numItems;
public:
  SourceFilter(tbb::filter::mode m, std::vector<int>& v, double u) 
      : filter(m), data(v), usec(u) {
    numItems = v.size() - 1;
  }
 
  void *operator() (void *) override {
    int i = numItems.fetch_and_add(-1);
    if ( i >= 0 ) {
      doWork(usec);
    } else {
      return NULL;
    }
    return (void *)&(data[i]);
  }
};

class SpinFilter : public tbb::filter {
  tbb::filter::mode mode;
  double usec;
public:
  SpinFilter() : filter(tbb::filter::parallel), 
                  mode(tbb::filter::parallel), usec(0) {} 
  SpinFilter(tbb::filter::mode m, double u) 
      : mode(m), filter(m), usec(u) { }
  SpinFilter(const SpinFilter& s) 
      : mode(s.mode), filter(mode), usec(s.usec) { }
 
  void *operator() (void *item) override {
    doWork(usec);
    return item;
  }
};

double fig_16_22(std::vector<int>& data, int num_filters, 
                 tbb::filter::mode m, double usec, int num_tokens, 
                 double imbalance, int num_items) {
  tbb::tick_count t0 = tbb::tick_count::now();
  tbb::pipeline p;
  SourceFilter s{m, data, usec};
  p.add_filter(s);
  std::vector<std::unique_ptr<SpinFilter>> sp_ptrs;
  for (int i = 0; i < num_filters - 1; ++i) {
    if (i == 2) {
      sp_ptrs.push_back(std::unique_ptr<SpinFilter>(new SpinFilter(m, imbalance * usec)));
    } else {
      sp_ptrs.push_back(std::unique_ptr<SpinFilter>(new SpinFilter(m, usec)));
    }
    p.add_filter(*sp_ptrs[i]);
  }
  p.run(num_tokens);
  double total_time = (tbb::tick_count::now() - t0).seconds();
  return total_time;   
}

double runSerial(std::vector<int>& data, int num_filters, double usec, 
                 double imbalance, int num_items) {
  tbb::tick_count t0 = tbb::tick_count::now();
  for (int i = 0; i < num_items; ++i) {
    for (int j = 0; j < num_filters; ++j) {
      if (j == 3) {
        doWork(imbalance * usec);
      } else {
        doWork(usec);
      }
    }
  }
  double total_time = (tbb::tick_count::now() - t0).seconds();
  return total_time;   
}

void doWork(double sec) {
  tbb::tick_count t0 = tbb::tick_count::now();
  while ((tbb::tick_count::now() - t0).seconds() <= sec);
}

std::vector<int> makeVector(int N) {
   std::vector<int> v;
   v.reserve(N);
   for (int i = 0; i < N; ++i) {
     v.push_back(i);
   }
   return v;
}

static void warmupTBB() {
  tbb::parallel_for(0, tbb::task_scheduler_init::default_num_threads(), [](int) {
    tbb::tick_count t0 = tbb::tick_count::now();
    while ((tbb::tick_count::now() - t0).seconds() < 0.01);
  });
}

int main() {
  int N = 8000;
  int max_threads = tbb::task_scheduler_init::default_num_threads();
  int num_filters = max_threads;
  int num_tokens = 8;
  std::vector<tbb::filter::mode> modes = {tbb::filter::serial_in_order, 
                                          tbb::filter::serial_out_of_order, 
                                          tbb::filter::parallel };
  std::vector<std::string> mode_name = {"serial_in_order", 
                                        "serial_out_of_order", 
                                        "parallel"};

  std::vector<int> data = makeVector(N);

  std::vector<double> imbalance_factor = {0.1, 0.5, 0.75, 1.5, 2, 10};
  double spin_time = 1e-4;

  std::cout << "Varying the imbalance given a usual spin time of 100 us:" << std::endl
            << "filter mode, 0.1, 0.5, 0.75, 1.5, 2, 10" << std::endl;

  std::vector<double> serial_time;
  for (double imbalance : imbalance_factor) 
    serial_time.push_back(runSerial(data, num_filters, spin_time, imbalance, N));

  for (int m = 0; m < modes.size(); ++m) {
    std::cout << mode_name[m];
    for (int i = 0; i < imbalance_factor.size(); ++i) {
      double imbalance = imbalance_factor[i];
      warmupTBB();
      double t = fig_16_22(data, num_filters, modes[m], spin_time, 
                           num_tokens, imbalance, N);
      std::cout << ", " << serial_time[i]/t;
    }
    std::cout << std::endl; 
  }
  return 0;
}

