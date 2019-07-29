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
#include <utility>
#include <tbb/task_scheduler_init.h>
#include <tbb/enumerable_thread_specific.h>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>

typedef tbb::enumerable_thread_specific< std::pair<int,int> > CounterType;
CounterType MyCounters (std::make_pair(0,0));

struct Body {
  void operator()(const tbb::blocked_range<int>& r) const {
    CounterType::reference my_counter = MyCounters.local();
    ++my_counter.first;
    for (int i = r.begin(); i != r.end(); ++i)
      ++my_counter.second;
  }
};

int main() {
  tbb::parallel_for(
		    tbb::blocked_range<int>(0, 100000000), Body());

  for (CounterType::const_iterator i = MyCounters.begin();
       i != MyCounters.end(); ++i) {
    printf("Thread stats:\n");
    printf("     calls to operator(): %d", i->first);
    printf("     total # of iterations executed: %d\n",
           i->second);
  }
  std::pair<int,int> sum =
    MyCounters.combine([](std::pair<int,int> x,
                          std::pair<int,int> y) {
			 return std::make_pair(x.first+y.first,
					       x.second+y.second);
		       });
  printf("Total calls to operator() = %d, "
	 "total iterations = %d\n", sum.first, sum.second);
  return 0;
}
