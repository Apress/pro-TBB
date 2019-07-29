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
#include <vector>

int N = 1 << 13;

double *setArray(int N, double *a);
void checkArray(int N, double *a);
void checkTranspose(int N, double *a);
double serialTranspose(int N, double *a, double *b);

struct FGMsg {
  int N;
  double *a;
  double *b;
  FGMsg() : N(0), a(0), b(0) {}
  FGMsg(int _N, double *_a, double *_b) : N(_N), a(_a), b(_b) {}
};

double fig_17_9(int N, double *a[3], double *b[3]) {
  tbb::tick_count t0 = tbb::tick_count::now();
  tbb::flow::graph g;
  int i = 0;
  tbb::flow::source_node<FGMsg> initialize{g, [&](FGMsg& msg) -> bool {
    if (i < 3) {
      msg = {N, setArray(N, a[i]), setArray(N, b[i])};
      ++i;
      return true;
    } else {
      return false; 
    }
  }, false};
  tbb::flow::function_node<FGMsg, FGMsg> transpose{g, tbb::flow::unlimited,
    [](const FGMsg& msg) -> FGMsg {
      serialTranspose(msg.N, msg.a, msg.b);
      return msg;
  }};
  tbb::flow::function_node<FGMsg> check{g, tbb::flow::unlimited,
    [](const FGMsg& msg) -> FGMsg {
    checkArray(msg.N, msg.b);
    return msg;
  }};
  tbb::flow::make_edge(initialize, transpose);
  tbb::flow::make_edge(transpose, check);
  initialize.activate();
  g.wait_for_all();
  double total_time = (tbb::tick_count::now() - t0).seconds();

  // a double check
  for (int i = 0; i < 3; ++i) {
    checkTranspose(N, b[i]);
  }
  return total_time;
}

double *setArray(int N, double *a) {
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      a[i*N + j] = i;
    }
  }
  return a;
}

void checkArray(int N, double *a) {
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      if (a[i*N + j] != j) {
        std::cout << "Transpose failed" << std::endl;
      }
    }
  }
}

void checkTranspose(int N, double *a) {
  return checkArray(N, a);
}

double serialTranspose(int N, double *a, double *b) {
  tbb::tick_count t0 = tbb::tick_count::now();
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      b[j*N + i] = a[i*N + j];
    }
  }
  tbb::tick_count t1 = tbb::tick_count::now();
  return (t1 - t0).seconds();
}


int main() {
  double *a[3];
  double *b [3];

  for (int i = 0; i < 3; ++i) {
    a[i] = new double[N*N];
    b[i] = new double[N*N];
  }

  fig_17_9(N, a, b);
  double t_fg_s = fig_17_9(N, a, b);
  std::cout << "Time (seconds) = " << t_fg_s << std::endl;
  return 0;
}

