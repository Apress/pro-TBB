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

#include <numeric>
#include <functional>
#include <tbb/parallel_reduce.h>
#include <tbb/blocked_range.h>

using namespace tbb;

int ParallelSumA( int array[], size_t n ) {
  return parallel_reduce(
			 blocked_range<int*>( array, array+n ),
			 0,
			 [](const blocked_range<int*>& r, int value)->int {
			   return std::accumulate(r.begin(),r.end(),value);
			 },
			 std::plus<int>()
			 );
}


int ParallelSumB( int array[], size_t n ) {
  return parallel_reduce(
			 blocked_range<int*>( array, array+n ),
			 0,
			 [](const blocked_range<int*>& r, int value)->int {
			   return std::accumulate(r.begin(),r.end(),value);
			 },
			 []( int x, int y )->int {
	printf("ADD(%d,%d)\n",x,y);
			   return x+y;
			 }
			 );
}


int main() {

  int x[] { 1,1,1,1,1, 1,1,1,1,1 };

  printf("%11d\n", ParallelSumA(x,10));

  printf("%11d\n", ParallelSumB(x,10));

  return 0;
}
