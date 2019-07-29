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

kernel void cl_print(global float* A,
                     global float* B,
                     global float* C,
                     int w) {
  int2 gId =    (int2)(get_global_id(0),   get_global_id(1));
  int2 lId =    (int2)(get_local_id(0),    get_local_id(1));
  int2 grId =   (int2)(get_group_id (0),   get_group_id (1));
  int2 gSize =  (int2)(get_global_size(0), get_global_size(1));
  int2 lSize =  (int2)(get_local_size(0),  get_local_size(1));
  int2 numGrp = (int2)(get_num_groups (0), get_num_groups (1));
  if(gId.x == 0 && gId.y==0)
    printf("gSize.x=%d, gSize.y=%d, lSize.x=%d, lSize.y=%d, numGrp.x=%d, numGrp.y=%d\n",
            gSize.x, gSize.y, lSize.x, lSize.y, numGrp.x, numGrp);
  printf("gId.x=%d, gId.y=%d, lId.x=%d, lId.y=%d, grId.x=%d, grId.y=%d, A=%f \n\n",
          gId.x, gId.y, lId.x, lId.y, grId.x, grId.y, A[gId.y*w+gId.x]);
  C[gId.y*w+gId.x] = A[gId.y*w+gId.x] + B[gId.y*w+gId.x];
}
