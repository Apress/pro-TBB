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

// Compile with: g++ -o p par_histo_coarse.cpp -ltbb -std=c++11 -O3
#include <vector>
#include <iostream>
#include <algorithm>

#include <tbb/task_scheduler_init.h>
#include <tbb/tick_count.h>
#include <tbb/atomic.h>
#include <tbb/parallel_invoke.h>


alignas(64) tbb::atomic<uint32_t> v{1};

void fetch_and_triple(tbb::atomic<uint32_t>& v)
{
  uint32_t old_v;
  do {
    old_v=v; //take a snapshot
  } while (v.compare_and_swap(old_v * 3, old_v)!=old_v);
}

int main(int argc, char** argv)
{
  long int N = 100; //1000000000;
  int nth = 2 ;

  std::cout<< "N="<< N << " and nth=" << nth << "\t";
  tbb::task_scheduler_init init{nth};

  tbb::parallel_invoke(
    [&](){for(int i=0; i<N; ++i) fetch_and_triple(v);},
    [&](){for(int i=0; i<N; ++i) fetch_and_triple(v);}
  );
  std::cout << " v=" <<v <<std::endl;

  return 0;
}
