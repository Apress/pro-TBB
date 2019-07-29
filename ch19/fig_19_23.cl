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
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.Â IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
OR OTHER DEALINGS IN THE SOFTWARE.

SPDX-License-Identifier: MIT
*/

kernel void cl_add(global float* A,global float* B,global float* C){
  int2 gId = (int2)(get_global_id(0), get_global_id(1));
  int2 gSz = (int2)(get_global_size(0), get_global_size(1));

  if(gId.x == 0 && gId.y==0)
    printf("gSz.x=%d, gSz.y=%d\n",gSz.x,gSz.y);
  C[gId.y*gSz.x+gId.x]=A[gId.y*gSz.x+gId.x]+B[gId.y*gSz.x+gId.x];
}

kernel void cl_sub(global float* A,global float* B,global float* C){
  int2 gId = (int2)(get_global_id(0), get_global_id(1));
  int2 gSz = (int2)(get_global_size(0), get_global_size(1));

  if(gId.x == 0 && gId.y==0)
    printf("gSz.x=%d, gSz.y=%d\n",gSz.x,gSz.y);
  C[gId.y*gSz.x+gId.x]=C[gId.y*gSz.x+gId.x]-B[gId.y*gSz.x+gId.x];
}
