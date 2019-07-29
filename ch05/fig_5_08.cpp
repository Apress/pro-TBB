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

#include <vector>
#include <iostream>
#include <algorithm>
#include <random>
#include <tbb/tick_count.h>
#include <tbb/parallel_for.h>
#include <tbb/task_scheduler_init.h>
#include <tbb/spin_mutex.h>

int main(int argc, char** argv) {

  long int n = 1000000000;
  int nth = 4;
  constexpr int num_bins = 256;

  // Initialize random number generator
  std::random_device seed;    // Random device seed
  std::mt19937 mte{seed()};   // mersenne_twister_engine
  std::uniform_int_distribution<> uniform{0,num_bins};
  // Initialize image  
  std::vector<uint8_t> image; // empty vector
  image.reserve(n);           // image vector prealocated
  std::generate_n(std::back_inserter(image), n,
                    [&] { return uniform(mte); }
                 );
  // Initialize histogram
  std::vector<int> hist(num_bins);

  tbb::task_scheduler_init init{nth};

  // Serial execution
  tbb::tick_count t0 = tbb::tick_count::now();
  std::for_each(image.begin(), image.end(),
                [&](uint8_t i){hist[i]++;});
  tbb::tick_count t1 = tbb::tick_count::now();
  double t_serial = (t1 - t0).seconds();

  // Parallel execution
  using my_mutex_t=tbb::spin_mutex;
  my_mutex_t my_mutex;
  std::vector<int> hist_p(num_bins);
  t0 = tbb::tick_count::now();
  parallel_for(tbb::blocked_range<size_t>{0, image.size()},
              [&](const tbb::blocked_range<size_t>& r)
              {
                my_mutex_t::scoped_lock my_lock{my_mutex};
                for (size_t i = r.begin(); i < r.end(); ++i)
                  hist_p[image[i]]++;
              });
  t1 = tbb::tick_count::now();
  double t_parallel = (t1 - t0).seconds();
  
  /* Not recommended alternative:
  parallel_for(tbb::blocked_range<size_t>{0, image.size()},
              [&](const tbb::blocked_range<size_t>& r)
              {
                my_mutex_t::scoped_lock my_lock;
                my_lock.acquire(my_mutex);
                for (size_t i = r.begin(); i < r.end(); ++i)
                  hist_p[image[i]]++;
                my_lock.release();
              });
     */

  
  std::cout << "Serial: "   << t_serial   << ", ";
  std::cout << "Parallel: " << t_parallel << ", ";
  std::cout << "Speed-up: " << t_serial/t_parallel << std::endl;

  if (hist != hist_p)
      std::cerr << "Parallel computation failed!!" << std::endl;
  return 0;
}
