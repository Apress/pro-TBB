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

#include <stdio.h>
#include <tbb/tbb.h>
#include <tbb/scalable_allocator.h>

// No retry loop because we assume that
// scalable_malloc does all it takes to allocate
// the memory, so calling it repeatedly
// will not improve the situation at all
//
// No use of std::new_handler because it cannot be
// done in portable and thread-safe way (see sidebar)
//
// We throw std::bad_alloc() when scalable_malloc
// returns NULL (we return NULL if it is a no-throw
// implementation)

void* operator new (size_t size) throw (std::bad_alloc)
{
  if (size == 0) size = 1; 
  if (void* ptr = scalable_malloc (size))
    return ptr;
  throw std::bad_alloc (  );
}

void* operator new[] (size_t size) throw (std::bad_alloc)
{
  return operator new (size);
}

void* operator new (size_t size, const std::nothrow_t&) throw (  )
{
  if (size == 0) size = 1; 
  if (void* ptr = scalable_malloc (size))
    return ptr;
  return NULL;
}

void* operator new[] (size_t size, const std::nothrow_t&) throw (  )
{
  return operator new (size, std::nothrow);
}

void operator delete (void* ptr) throw (  )
{
  if (ptr != 0) scalable_free (ptr);
}

void operator delete[] (void* ptr) throw (  )
{
  operator delete (ptr);
}

void operator delete (void* ptr, const std::nothrow_t&) throw (  )
{
  if (ptr != 0) scalable_free (ptr);
}

void operator delete[] (void* ptr, const std::nothrow_t&) throw (  )
{
  operator delete (ptr, std::nothrow);
}

/****************/
/****************/
/****************/
/****************/

const int N = 500000;

int main() {
  double *a[N];

  tbb::parallel_for( 0, N-1, [&](int i) { a[i] = new double; } );
  tbb::parallel_for( 0, N-1, [&](int i) { delete a[i];       } );

  return 0;
}
