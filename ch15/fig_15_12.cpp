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

#include <tbb/tbb.h>
#include <iostream>

#define N 100

bool data[N][N];
int main(){
  try {
    tbb::parallel_for(0, N, 1,
      [&](int i){
        tbb::task_group_context root{tbb::task_group_context::isolated};
        tbb::parallel_for(0, N, 1,
          [&](int j){
            data[i][j] = true;
          }
          //, root //Uncomment and see!
        );
        throw "oops";
      });
  }
  catch(...) {
    std::cout << "An exception captured " << std::endl;
  }

  for(int i=0; i<N ; i++)
    for(int j=0;  j<N; j++){
      if(data[i][j]) std::cout << i << "  " << j << " : true " << std::endl;
      //if(!data[i][j]) cout<< i << "  " << j << " : false " << endl;
    }
  return 0;
}
