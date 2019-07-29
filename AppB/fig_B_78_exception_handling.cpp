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

#include <tbb/tbb_exception.h>
#include <tbb/parallel_for.h>
#include <vector>
#include <iostream>
 
std::vector<int> Data;
 
struct Update {
  void operator()( const tbb::blocked_range<int>& r ) const {
    for( int i=r.begin(); i!=r.end(); ++i )
      Data.at(i) += 1;
  }
};
 
int main( int argc, char *argv[] ) {
  int vecsize = 1000;
  int vecwalk = vecsize;
  Data.resize(vecsize);
  printf("Vector of size %d being traversed up to element %d\n",
	 vecsize,vecwalk);
  try {
    tbb::parallel_for( tbb::blocked_range<int>(0, vecwalk), Update());
  } catch( std::out_of_range& ex ) {
    std::cout << "Exception caught was out_of_range: " << 
      ex.what() << std::endl;
    exit(0);
  }
  printf("No exception detected.\n");
  return 0;
}
