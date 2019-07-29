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

std::vector<double> a_par;
std::vector<std::atomic<int>> counters;
int n, gs;

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
public:
  Cell(int i_ ,int j_) : i{i_},j{j_} {}
  task* execute(){
    a_par[i*n+j] = foo(gs, a_par[i*n+j], a_par[(i-1)*n+j], a_par[i*n+j-1]);
    bool recycle_into_east=false;
    bool recycle_into_south=false;
    if (j<n-1 && --counters[i*n+j+1]==0) recycle_into_east=true;
    if (i<n-1 && --counters[(i+1)*n+j]==0){
        if (!recycle_into_east) recycle_into_south = true;
        else
          spawn(*new(allocate_additional_child_of(*parent()))
            Cell{i+1,j});
      }
    if (recycle_into_east) {
      recycle_as_child_of(*parent());
      j = j+1;
      return this;
    }
    else if(recycle_into_south){
      recycle_as_child_of(*parent());
      i=i+1;
      return this;
    }
    else return nullptr;
  }
};

int main (int argc, char **argv)
{
  n = 1000;
  int nth= 4;
  gs= 50;

  int size = n*n;
  std::vector<double> a_ser(size);
  a_par.resize(size);
  counters = std::vector<std::atomic<int>>(size);

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

  //Initialize matrix of counters
  for(int i=0; i<n; i++)
    for (int j=0; j<n; j++)
      if (i == 1 || j==1) {
        counters[i*n+j]=1;
      }
      else {
        counters[i*n+j]=2;
      }
  counters[n+1] = 0;

  tbb::task_scheduler_init init(nth);
  common::warmupTBB(0.01, nth);

  t0 = tbb::tick_count::now();
  tbb::task::spawn_root_and_wait(*new(tbb::task::allocate_root())
                                 Cell{1,1});
  t1 = tbb::tick_count::now();
  auto t_par = (t1-t0).seconds()*1000;

  if (a_ser != a_par)
      std::cerr << "Parallel computation failed!!" << std::endl;

  std::cout<<"Serial Time = " << t_ser <<" msec\n";
  std::cout<<"Thrds = " << nth << "; Parallel Time = " << t_par << " msec\n";
  std::cout<<"Speedup = " << t_ser/t_par << '\n';

  return 0;
}
