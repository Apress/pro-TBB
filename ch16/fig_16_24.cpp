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
#include <limits>
#include <random>
#include <vector>
#include <tbb/tbb.h>

struct SortData {
  int id;
  double value;
  SortData(int i, double v) : id(i), value(v) {}
  bool operator<(const SortData& other) const {
    return value < other.value;
  }
  bool operator==(const SortData& other) const {
    return value == other.value;
  }
};

using QSVector = std::vector<SortData>;

QSVector::iterator doShuffle(QSVector::iterator b, 
                             QSVector::iterator e) {
  QSVector::iterator i = b, j = e-1;
  double pivot_value = b->value;
  while (i != j) {
    while (i != j && pivot_value < j->value) --j;
    while (i != j && i->value <= pivot_value) ++i;
    std::iter_swap(i, j);
  }
  std::iter_swap(b, i);
  return i;
}

void serialQuicksort(QSVector::iterator b, 
                     QSVector::iterator e) {
  if (b >= e) return;
  QSVector::iterator i = doShuffle(b,e);
  serialQuicksort(b, i);
  serialQuicksort(i+1, e);
}

class ShuffleRange {
  QSVector::iterator myBegin;
  QSVector::iterator myEnd;

public:
  static const bool is_splittable_in_proportion = false;
  static const int cutoff = 100;

  // constructors
  ShuffleRange(const QSVector::iterator b, 
                const QSVector::iterator e ) 
    : myBegin(b), myEnd(e) { }

  ShuffleRange(const ShuffleRange& r) 
    : myBegin(r.myBegin), myEnd(r.myEnd) { }

  ShuffleRange(ShuffleRange& r, tbb::split)
      : myBegin(r.myBegin), myEnd(r.myEnd) {
	QSVector::iterator b = r.myBegin;
	QSVector::iterator e = r.myEnd;
	QSVector::iterator i = doShuffle(b,e);
	r.myEnd = i;
	myBegin = i+1;
  }

  bool empty() const { return myBegin >= myEnd; }
  bool is_divisible() const { return myEnd-myBegin >= cutoff; }
  QSVector::iterator begin() const { return myBegin; }
  QSVector::iterator end() const { return myEnd; }
};

void pforQuicksort(QSVector::iterator b, QSVector::iterator e) {
  tbb::parallel_for(ShuffleRange(b, e), 
    [](const ShuffleRange& r) {
      serialQuicksort(r.begin(), r.end());
    }, 
    tbb::simple_partitioner() 
  );
}

std::vector<SortData> makeSortData(int N) {
    std::vector<SortData> v;

    std::default_random_engine g;
    std::uniform_real_distribution<double> d(0.0, 1.0);

    for (int i = 0; i < N; ++i)
        v.push_back(SortData{i, d(g)});

    return v;
}

void checkForSorted(const QSVector &v) {
  double max_value = v.front().value;
  for (auto e : v) {
    if (e.value < max_value) {
      std::cout << "Sort FAILED" << std::endl;
      abort();
    }
    max_value = e.value;
  }
}

int main() {
  const int N = 1000000;
  QSVector v = makeSortData(N);

  tbb::tick_count t0 = tbb::tick_count::now();
  pforQuicksort(v.begin(), v.end());
  double parallel_time = (tbb::tick_count::now() - t0).seconds();

  checkForSorted(v);

  std::cout << "parallel time == " << parallel_time << std::endl;
  return 0;
}

