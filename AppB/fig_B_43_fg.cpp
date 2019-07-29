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

/** Minimal Hello World. Created by James Reinders **/

#include <cstdio>
#include <tbb/tbb.h>

tbb::spin_mutex mylock;
int counter = 0;

struct hw {
  char myhw1, myhw2, myhw3;
  hw(const char hw1,const char hw2,const char hw3) :
    myhw1(hw1),myhw2(hw2),myhw3(hw3) {}
  void operator()(tbb::flow::continue_msg) const {
    int mycount;
    { tbb::spin_mutex::scoped_lock hello(mylock);
      mycount = counter++;
    }
    printf("%c",
	   (mycount < 5) ? myhw1 : (mycount < 10) ? myhw2 : myhw3);
  }
};

int main() {
  tbb::flow::graph g;

  tbb::flow::broadcast_node<tbb::flow::continue_msg> start(g);
#define X tbb::flow::continue_node<tbb::flow::continue_msg>
  X mya(g,hw('H',',','l'));
  X myb(g,hw('>','o','r'));
  X myc(g,hw('l','W','!'));
  X myd(g,hw('l','o','<'));
  X mye(g,hw('e',' ','d'));

  /*  tbb::flow::make_edge(start, mya); */
  tbb::flow::make_edge(start, myb);
  tbb::flow::make_edge(myb, mya);
  /*  tbb::flow::make_edge(myb, myc); */
  tbb::flow::make_edge(myc, myd);
  tbb::flow::make_edge(mya, mye);
  tbb::flow::make_edge(mye, myc);

  for (int i = 0; i < 3; ++i) {
    start.try_put(tbb::flow::continue_msg());
    g.wait_for_all();
  }

  printf("\n");

  return 0;
}
