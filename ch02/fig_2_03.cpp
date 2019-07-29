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

// avoid Windows macros
#define NOMINMAX

#include <iostream>
#include <vector>
#include <limits>
#include <tbb/tbb.h>

struct DataItem {
  int id;
  double value;
  DataItem(int i, double v) : id{i}, value{v} {}
};

using QSVector = std::vector<DataItem>;

static void serialQuicksort(QSVector::iterator b, QSVector::iterator e);

void fig_2_3(QSVector& v1, QSVector& v2) {
  tbb::parallel_invoke(
    [&v1]() { serialQuicksort(v1.begin(), v1.end()); },
    [&v2]() { serialQuicksort(v2.begin(), v2.end()); }
  );
}

#include <random>
#include <cfloat>

static QSVector makeQSData(int N) {
  QSVector v;

  std::default_random_engine g;
  std::uniform_real_distribution<double> d(0.0, 1.0);

  for (int i = 0; i < N; ++i)
    v.push_back(DataItem{i, d(g)});

  return v;
}

static bool checkIsSorted(const QSVector& v) {
  double max_value = std::numeric_limits<double>::min();
  for (auto e : v) {
    if (e.value < max_value) {
      std::cerr << "Sort FAILED" << std::endl;
      return false;
    }
    max_value = e.value;
  }
  return true;
}

static void serialQuicksort(QSVector::iterator b, QSVector::iterator e) {
  if (b >= e) return;

  // do shuffle
  double pivot_value = b->value;
  QSVector::iterator i = b, j = e-1;
  while (i != j) {
    while (i != j && pivot_value < j->value) --j;
    while (i != j && i->value <= pivot_value) ++i;
    std::iter_swap(i, j);
  }
  std::iter_swap(b, i);

  // recursive call
  serialQuicksort(b, i);
  serialQuicksort(i+1, e);
}

static void warmupTBB() {
  tbb::parallel_for(0, tbb::task_scheduler_init::default_num_threads(), [](int) {
    tbb::tick_count t0 = tbb::tick_count::now();
    while ((tbb::tick_count::now() - t0).seconds() < 0.01);
  });
}

int main(int argc, char *argv[]) {
  const int N = 1000000;

  QSVector serial_v1 = makeQSData(N);
  QSVector serial_v2 = makeQSData(N);
  QSVector tbb_v1 = makeQSData(N);
  QSVector tbb_v2 = makeQSData(N);

  double serial_time = 0.0, parallel_time = 0.0;
  {
    tbb::tick_count t0 = tbb::tick_count::now();
    serialQuicksort(serial_v1.begin(), serial_v1.end());
    serialQuicksort(serial_v2.begin(), serial_v2.end());
    serial_time = (tbb::tick_count::now() - t0).seconds();
    if (!checkIsSorted(serial_v1) || !checkIsSorted(serial_v2)) {
      std::cerr << "ERROR: serial sorted list out-of-order" << std::endl;
    }
  }

  warmupTBB();
  {
    tbb::tick_count t0 = tbb::tick_count::now();
    fig_2_3(tbb_v1, tbb_v2);
    parallel_time = (tbb::tick_count::now() - t0).seconds();
    if (!checkIsSorted(tbb_v1) || !checkIsSorted(tbb_v2)) {
      std::cerr << "ERROR: tbb sorted list out-of-order" << std::endl;
    }
  }

  std::cout << "serial_time == " << serial_time << " seconds" << std::endl
            << "parallel_time == " << parallel_time << " seconds" << std::endl
            << "speedup == " << serial_time/parallel_time << std::endl;

  return 0;
}

