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

#include <tbb/parallel_for.h>
#include <tbb/combinable.h>
#include <iostream>
#include <stdio.h>

#define HOWMANY 10

void dump(tbb::combinable<int> *pTLS) {
  tbb::parallel_for( 0, HOWMANY, [&](int i){
      bool truth = false;
      int val = 0;
      val = pTLS->local(truth);
      printf("%d%c ",val,truth ? 'T':'F');
    } );
  printf("\n");
}

int main() {
  int gval;
  tbb::combinable<int> myTLS([](){return 0;});
  tbb::combinable<int> mycopiedTLS([](){return 6;});

  dump( &myTLS );

  myTLS.clear();
  printf("cleared\n");

  dump( &myTLS );  dump( &myTLS );
  tbb::parallel_for( 0, HOWMANY, [&](int i){
      myTLS.local() += i;
    } );
  printf("added local values into local sums\n");
  dump( &myTLS );

  gval = myTLS.combine([](int a,int b){return a+b;});
  printf("global value = %d\n",gval);
  mycopiedTLS = myTLS;
  gval = mycopiedTLS.combine([](int a,int b){return a+b;});
  printf("global copied value = %d\n",gval);

  myTLS.clear();
  printf("cleared\n");

  gval = myTLS.combine([](int a,int b){return a+b;});
  printf("global value = %d\n",gval);
  gval = mycopiedTLS.combine([](int a,int b){return a+b;});
  printf("global copied value = %d\n",gval);
  mycopiedTLS.combine_each([](int a){printf("%d ",a);});
  printf("<< values from combine_each\n");
  gval = mycopiedTLS.combine([](int a,int b){return a+b;});
  printf("global copied value = %d\n",gval);
  return 0;
}
