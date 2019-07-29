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
tbb::atomic<int> counter = 0;

void hw(int x, int v) {
  int mycount = counter++;
  printf("Hello, Stage %d (%02d):(%02d)\n",x,mycount,v);
}

int main( int argc, char *argv[] ) {
  int cdown = 3;
  tbb::parallel_pipeline(
    6,
    tbb::make_filter<void,int>(
       tbb::filter::parallel,[&](tbb::flow_control& fc)->int
                                          { hw(1,0);
                                            if (!cdown) { fc.stop(); return 0; }
                                            return 1000 * cdown--;  }) &
    tbb::make_filter<int,float>(
       tbb::filter::parallel,[](int i)    { hw(2,i);      return ++i;     }) &
    tbb::make_filter<float,float>(
       tbb::filter::parallel,[](float f)  { hw(3,(int)f); return f+1.0f;  }) &
    tbb::make_filter<float,int>(
       tbb::filter::parallel,[](float f)  { hw(4,(int)f); return (int)f+1;}) &
    tbb::make_filter<int,void>(
       tbb::filter::parallel,[](int i)    { hw(5,i);                      })
  );
  return 0;
}
