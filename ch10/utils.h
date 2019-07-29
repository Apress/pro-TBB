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

/*
  Copyright 2018 Intel Corporation.  All Rights Reserved.

  The source code contained or described herein and all documents related
  to the source code ("Material") are owned by Intel Corporation or its
  suppliers or licensors.  Title to the Material remains with Intel
  Corporation or its suppliers and licensors.  The Material is protected
  by worldwide copyright laws and treaty provisions.  No part of the
  Material may be used, copied, reproduced, modified, published, uploaded,
  posted, transmitted, distributed, or disclosed in any way without
  Intel's prior express written permission.

  No license under any patent, copyright, trade secret or other
  intellectual property right is granted to or conferred upon you by
  disclosure or delivery of the Materials, either expressly, by
  implication, inducement, estoppel or otherwise.  Any license under such
  intellectual property rights must be express and approved by Intel in
  writing.
*/

#pragma once

#include "tbb/tbb.h"

#include <chrono>
#include <iostream>

namespace common {

class Timer {
  std::chrono::time_point<std::chrono::system_clock> startTime;    
  bool reportAtDestruction;

public:
  Timer(bool report=true) : startTime{std::chrono::system_clock::now()}, reportAtDestruction{report} {}

  virtual ~Timer() {
    if (reportAtDestruction) 
      std::cout << "Timer : " << elapsed_seconds() << " seconds" << std::endl;
  }

  double elapsed_seconds() const {
    std::chrono::duration<double> elapsed = std::chrono::system_clock::now() - startTime;
    return elapsed.count();
  } 

  int64_t elapsed_nanoseconds() const {
    std::chrono::duration<int64_t, std::nano> elapsed = std::chrono::system_clock::now() - startTime;
    return elapsed.count();
  } 

  void reset() {
    startTime = std::chrono::system_clock::now();
  }
};

static void spinWaitForAtLeast(double sec) {
  if (sec == 0.0) return;
  tbb::tick_count t0 = tbb::tick_count::now();
  while ((tbb::tick_count::now() - t0).seconds() < sec);
}

static void warmupTBB(double per_body_time = 0.01, int P = tbb::task_scheduler_init::default_num_threads()) {
  tbb::parallel_for(0, P, [per_body_time](int) {
    spinWaitForAtLeast(per_body_time);
  });
}

}

