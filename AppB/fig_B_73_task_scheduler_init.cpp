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

#include <tbb/task_scheduler_init.h>

#define XYZZY(FLAG) \
  printf(FLAG " default threads = %d; ", \
	 tbb::task_scheduler_init::default_num_threads()); \
  printf(my_init.is_active()?"TBB activate\n":"TBB not active\n");

int main( int argc, char *argv[] ) {
  {
    tbb::task_scheduler_init
      my_init(tbb::task_scheduler_init::deferred);
    XYZZY("AA");   my_init.initialize(10);
    XYZZY("AB");
  }
  {
    tbb::task_scheduler_init my_init;
    XYZZY("BB");
    // if this is used:
    // my_init.initialize(24);
    // the runtime will fault, printing something like this:
    // Assertion !my_scheduler failed...
    // Detailed description: task_scheduler_init already initialized
    // Abort trap: 6
    XYZZY("BC");   my_init.terminate();
    XYZZY("BD");   my_init.initialize(90);
    XYZZY("BE");
  }
  return 0;
}
