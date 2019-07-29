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

#include "utils.h"
#include <tbb/tbb.h>

void sidebar_associativity(int N) {
  std::cout << "N == " << N << std::endl;

  float r = 0.0;
  for (uint64_t i = 0; i < N; ++i) 
    r += 1.0;
  std::cout << "in-order sum == " << r << std::endl;

  float tmp1 = 0.0, tmp2 = 0.0;
  for (uint64_t i = 0; i < N/2; ++i)
    tmp1 += 1.0;
  for (uint64_t i = N/2; i < N; ++i)
    tmp2 += 1.0;
  float r2 = tmp1 + tmp2;
  std::cout << "associative sum == " << r2 << std::endl;
}

int main(int argc, char *argv[]) {
  sidebar_associativity(10e6);
  sidebar_associativity(20e6);
  return 0;
}

