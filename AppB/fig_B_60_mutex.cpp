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
#include <tbb/tick_count.h>
#include <tbb/mutex.h>
#include <tbb/recursive_mutex.h>
#include <tbb/spin_mutex.h>
#include <tbb/queuing_mutex.h>
#include <tbb/spin_rw_mutex.h>
#include <tbb/queuing_rw_mutex.h>
#include <tbb/null_mutex.h>
#include <tbb/null_rw_mutex.h>

int main( int argc, char *argv[] ) {
  tbb::mutex                            my_mutex_01;
  tbb::recursive_mutex                  my_mutex_02;
  tbb::spin_mutex                       my_mutex_03;
  tbb::speculative_spin_mutex           my_mutex_04;
  tbb::queuing_mutex                    my_mutex_05;
  tbb::spin_rw_mutex                    my_mutex_06;
  tbb::speculative_spin_rw_mutex        my_mutex_07;
  tbb::queuing_rw_mutex                 my_mutex_08;
  tbb::null_mutex                       my_mutex_09;
  tbb::null_rw_mutex                    my_mutex_10;
  {
    tbb::mutex::scoped_lock                     mylock01a(my_mutex_01);
    // the following would stall because already locked the mutex
    // tbb::mutex::scoped_lock                  mylock01b(my_mutex_01);
    tbb::recursive_mutex::scoped_lock           mylock02a(my_mutex_02);
    // recursion is allowed... so this does not stall...
    tbb::recursive_mutex::scoped_lock           mylock02b(my_mutex_02);
    // recursion is allowed... so this does not stall...
    tbb::recursive_mutex::scoped_lock           mylock02c(my_mutex_02);
    tbb::spin_mutex::scoped_lock                mylock03a(my_mutex_03);
    tbb::speculative_spin_mutex::scoped_lock    mylock04a(my_mutex_04);
    // reader...
    tbb::spin_rw_mutex::scoped_lock             mylock06a(my_mutex_06);
    // the following would stall, really!
    // we already have locked the mutex from this thread
    // RW allows one lock per thread - this is not a recursive lock!
    // tbb::spin_rw_mutex::scoped_lock          mylock06c(my_mutex_06);
    tbb::speculative_spin_rw_mutex::scoped_lock mylock07a(my_mutex_07);
    // writer...
    tbb::queuing_rw_mutex::scoped_lock          mylock08a(my_mutex_08,true);
    tbb::null_mutex::scoped_lock                mylock09a(my_mutex_09);
    // null does nothing... so this does not stall...
    tbb::null_mutex::scoped_lock                mylock09b(my_mutex_09);
    tbb::null_rw_mutex::scoped_lock             mylock10a(my_mutex_10);
    // null does nothing... so this does not stall...
    tbb::null_rw_mutex::scoped_lock             mylock10b(my_mutex_10);
    // null does nothing... so this does not stall...
    tbb::null_rw_mutex::scoped_lock             mylock10c(my_mutex_10);

    printf("Locks acquired!\nHello, World!\n");
  }
  return 0;
}
