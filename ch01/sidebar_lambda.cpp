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

void printValues(int i, int j, int k, int l) {
  std::cout << "i == " << i << std::endl
            << "j == " << j << std::endl
            << "k == " << k << std::endl
            << "l == " << l << std::endl;
}

int main(int argc, char *argv[]) {
  int i = 1, j = 10, k = 100, l = 1000;
  auto lambdaExpression = [i, &j] (int k0, int& l0) -> int {
    j = 2 * j;
    k0 = 2 * k0;
    l0 = 2 * l0;
    return i + j + k0 + l0;
  };

  printValues(i, j, k, l);
  std::cout << "First call returned " << lambdaExpression(k, l) << std::endl;
  printValues(i, j, k, l);
  std::cout << "Second call returned " << lambdaExpression(k, l) << std::endl;
  printValues(i, j, k, l);
  return 0;
}

