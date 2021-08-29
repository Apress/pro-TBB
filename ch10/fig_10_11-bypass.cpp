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
// NOTE: low-level task API REMOVED in oneTBB
#include <tbb/tick_count.h>

constexpr int cutoff = 30;

long fib(long n) {
  if(n<2)
    return n;
  else
    return fib(n-1)+fib(n-2);
}

int main(int argc, char** argv)
{
  int n = 40;


  auto t0 = tbb::tick_count::now();
  long fib_s = fib(n);
  auto t1 = tbb::tick_count::now();

  double t_s = (t1 - t0).seconds();

  std::cout << "SerialFib:   " << fib_s << " Time: " << t_s << "\n";
  std::cout << "ParallelFib not computed. Task API removed.\n";
  std::cout << "Task_group alternative can be used following a bypassing approach,\n";
  std::cout << "but Fibonacci example can not be implemented directly with bypassing.\n";
  std::cout << "See: https://docs.oneapi.io/versions/latest/onetbb/tbb_userguide/Migration_Guide/Task_API.html\n";
  return 0;
}

