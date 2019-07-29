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

#include <cstdio>
#include <iostream>
#include <tbb/tbb.h>
//#include <asm/cachectl.h>

int main(int argc, const char* argv[]) {

  int nth = 4;
  size_t vsize = 100000000;
  float alpha = 0.5;
  tbb::task_scheduler_init init{nth};

  std::unique_ptr<double[]> A{new double[vsize]};
  std::unique_ptr<double[]> B{new double[vsize]};
  std::unique_ptr<double[]> C{new double[vsize]};

  for(size_t i = 0; i < vsize; i++){
    A[i] = B[i] = i;
  }
  //cacheflush((char*)A, vsize*sizeof(double), DCACHE);

  auto t=tbb::tick_count::now();
  tbb::parallel_for(tbb::blocked_range<size_t>{0, vsize},
    [&](const tbb::blocked_range<size_t>& r){
      for (size_t i = r.begin(); i < r.end(); ++i)
        C[i] = A[i] + alpha * B[i];
    });
  double ts = (tbb::tick_count::now() - t).seconds();

#ifdef VERBOSE
  std::cout << "Results: " << '\n';
  for (size_t i = 0; i < vsize; i++){
    std::cout << C[i] << ", ";
  }
  std::cout << '\n';
#endif

  std::cout << "Time: " << ts << " seconds; ";
  std::cout << "Bandwidth: "  << vsize*24/ts/1000000.0 << "MB/s\n";
  return 0;
}
