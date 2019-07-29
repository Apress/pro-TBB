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
#include <tbb/task.h>
#include <tbb/task_scheduler_init.h>
#include <atomic>
/*#include <unistd.h>*/
#include <vector>
#include "utils.h"

double foo (int gs, double a, double b, double c){
      double x = (a + b + c)/3;
      //common::spinWaitForAtLeast(gs*(double)1.0e-9);
      int dummy=0;
      for (int i=0; i<gs; i++) dummy += (a + b + c)/4;
      //avoid dead code elimination:
      if (!dummy) common::spinWaitForAtLeast((dummy+1)*1e-9);
      return x;
}

//Task class
class Cell: public tbb::task {
  int i,j;
  int n;
  int gs;
  std::vector<double>& A;
public:
  // successor[0] = south successor, successor[1] = east successor
  Cell* successor[2];
  Cell(int i_ ,int j_, int n_, int gs_,
       std::vector<double>& A_) :
       i{i_},j{j_},n{n_},gs{gs_},A{A_} {}
  task* execute(){
    A[i*n+j] = foo(gs, A[i*n+j], A[(i-1)*n+j], A[i*n+j-1]);
    for( int k=0; k<2; ++k )
      if(Cell* t = successor[k])
        if(t->decrement_ref_count()==0)
          spawn(*t);
    return nullptr;
  }
};

int main (int argc, char **argv)
{
  int n = 1000;
  int nth= 4;
  int gs= 50;

  int size = n*n;
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

  tbb::task_scheduler_init init(nth);
  common::warmupTBB(0.01, nth);
  // Build DAG of Cells
  std::vector<Cell*> DAG(size);
  for( int i=n; --i>0; )
      for( int j=n; --j>0; ) {
          DAG[i*n+j] = new(tbb::task::allocate_root())
                          Cell(i,j,n,gs,a_par);
          DAG[i*n+j]->successor[0] = i+1<n ? DAG[(i+1)*n+j] : nullptr;
          DAG[i*n+j]->successor[1] = j+1<n ? DAG[i*n+j+1] : nullptr;
          DAG[i*n+j]->set_ref_count((i>1)+(j>1));
      }
  // Add extra reference to last task, because it is waited on
  // by spawn_and_wait_for_all.
  DAG[size-1]->increment_ref_count();

  t0 = tbb::tick_count::now();
  // Wait for all but last task to complete.
  DAG[size-1]->spawn_and_wait_for_all(*DAG[n+1]);
  // Last task is not executed implicitly, so execute it explicitly.
  DAG[size-1]->execute();
  // Destroy last task.
  tbb::task::destroy(*DAG[size-1]);
  t1 = tbb::tick_count::now();
  auto t_par = (t1-t0).seconds()*1000;

  if (a_ser != a_par)
      std::cerr << "Parallel computation failed!!" << std::endl;

  std::cout<<"Serial Time = " << t_ser <<" msec\n";
  std::cout<<"Thrds = " << nth << "; Parallel Time = " << t_par << " msec\n";
  std::cout<<"Speedup = " << t_ser/t_par << '\n';

  return 0;
}
