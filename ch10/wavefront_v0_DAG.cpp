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
#include <tbb/tick_count.h>
// NOTE: task API is removed. 
// DAG of tasks has been replaced by a DAG of FlowGraph continue_node
#include <tbb/flow_graph.h>
#include <tbb/global_control.h>
#include <atomic>
/*#include <unistd.h>*/
#include <vector>
#include "utils.h"

using Cell = tbb::flow::continue_node<tbb::flow::continue_msg>;
using CellPtr = std::shared_ptr<Cell>;

double foo (int gs, double a, double b, double c){
      double x = (a + b + c)/3;
      //common::spinWaitForAtLeast(gs*(double)1.0e-9);
      int dummy=0;
      for (int i=0; i<gs; i++) dummy += (a + b + c)/4;
      //avoid dead code elimination:
      if (!dummy) common::spinWaitForAtLeast((dummy+1)*1e-9);
      return x;
}

CellPtr createCell(tbb::flow::graph& g, int i, int j, int n, int gs, 
                   std::vector<double>& A) {
  return std::make_shared<Cell>(g,
    [i, j, n, gs, &A] (const tbb::flow::continue_msg& msg) {
      A[i*n+j] = foo(gs, A[i*n+j], A[(i-1)*n+j], A[i*n+j-1]);
      return msg;
    }
  );
}

void addEdges(std::vector<CellPtr>& cells, int i, int j, int n) {
  CellPtr cp = cells[i*n + j];
  if (j + 1 < n)
    tbb::flow::make_edge(*cp, *cells[i*n + j + 1]);
  if (i + 1 < n)
    tbb::flow::make_edge(*cp, *cells[(i + 1)*n + j]);
}

int main (int argc, char **argv)
{
  constexpr int n = 1000;
  constexpr size_t nth = 4;
  constexpr int gs = 50;

  constexpr int size = n*n;
  std::vector<double> a_ser(size);
  std::vector<double> a_par(size);

  //Initialize a_ser & a_par with dummy values
  for(int i=0; i<size; i++)
      a_ser[i] = a_par[i] = i%300+1000.0;

  //Serial execution
  auto t0 = tbb::tick_count::now();
  for (int i=1; i<n; ++i)
    for (int j=1; j<n; ++j)
      a_ser[i*n+j] =foo(gs, a_ser[i*n+j],
                            a_ser[(i-1)*n+j],
                            a_ser[i*n+j-1]);
  auto t1 = tbb::tick_count::now();
  auto t_ser = (t1-t0).seconds()*1000;

  tbb::global_control global_limit{tbb::global_control::max_allowed_parallelism, nth};
  common::warmupTBB(0.01, nth);
  // Build DAG of Cells
  std::vector<CellPtr> cells(size);
  tbb::flow::graph g;
  for (int i = n - 1; i > 0; --i) {
    for (int j = n - 1; j > 0; --j) {
      cells[i*n + j] = createCell(g, i, j, n, gs, a_par);
      addEdges(cells, i, j, n);
    }
  }

  t0 = tbb::tick_count::now();
  // Wait for all tasks to complete.
  cells[n+1]->try_put(tbb::flow::continue_msg{});
  g.wait_for_all();
  t1 = tbb::tick_count::now();
  auto t_par = (t1-t0).seconds()*1000;

  if (a_ser != a_par)
      std::cerr << "Parallel computation failed!!" << std::endl;

  std::cout<<"Serial Time = " << t_ser <<" msec\n";
  std::cout<<"Thrds = " << nth << "; Parallel Time = " << t_par << " msec\n";
  std::cout<<"Speedup = " << t_ser/t_par << '\n';

  return 0;
}
