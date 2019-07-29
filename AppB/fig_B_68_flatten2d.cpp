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
#include <utility>
#include <vector>
#include <tbb/task_scheduler_init.h>
#include <tbb/enumerable_thread_specific.h>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
// A VecType has a separate std::vector<int> per thread
typedef
tbb::enumerable_thread_specific< std::vector<int> >
VecType;

VecType MyVectors; 
int k = 10000000; // made 10X larger to keep sample output short for the book
 
struct Func {
  void operator()(const tbb::blocked_range<int>& r) const {
    VecType::reference v = MyVectors.local();
    for (int i=r.begin(); i!=r.end(); ++i) 
      if( i%k==0 ) 
	v.push_back(i);
  } 
};
 
int main() {
  tbb::parallel_for(
		    tbb::blocked_range<int>(0, 100000000), Func());
 
  tbb::flattened2d<VecType> flat_view =
    tbb::flatten2d( MyVectors );

  for( tbb::flattened2d<VecType>::const_iterator 
         i = flat_view.begin(); i != flat_view.end(); ++i) 
    std::cout << *i << std::endl;

  return 0;
}
