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

#include <cmath>
#include <iostream>
#include <list>
#include <map>
#include <random>
#include <utility>
#include <vector>
#include <tbb/tbb.h>

using PrimesValue = std::pair<int, bool>;

struct PrimesTreeElement {
  using Ptr = std::shared_ptr<PrimesTreeElement>; 

  PrimesValue v;
  Ptr left;
  Ptr right;
  PrimesTreeElement(const PrimesValue& _v) : left{}, right{} {
    v.first = _v.first;
    v.second = _v.second;
  }
};

bool isPrime(int n);

void fig_2_19(PrimesTreeElement::Ptr root) {
  PrimesTreeElement::Ptr tree_array[] = {root};
  tbb::parallel_do(tree_array,
    [](PrimesTreeElement::Ptr e, 
      tbb::parallel_do_feeder<PrimesTreeElement::Ptr>& feeder) {
        if (e) {
          if (e->left) feeder.add(e->left);
          if (e->right) feeder.add(e->right);
          if (isPrime(e->v.first))
            e->v.second = true;
        }
      } 
  );
}

bool isPrime(int n) {
  int e =  std::sqrt(n);
  std::vector<bool> p(e+1, true);

  for (int i = 2; i <= e; ++i) {
    if (p[i]) {
      if (n % i) {
        for (int j = 2*i; j <= e; j += i) {
          p[j] = false;
        }
      } else {
        return false;
      }
    }
  }
  return true;
}

using PrimesMap = std::map<int, bool>;
using IntVector = std::vector<int>;

static IntVector makePrimesValues(int n, PrimesMap& m) {
  std::default_random_engine gen;
  std::uniform_int_distribution<int> dist;
  IntVector vec;

  for (int i = 0; i < n; ++i) {
    int v = dist(gen);
    vec.push_back( v );
    m[v] = isPrime(v);
  }
  return vec;
}

static PrimesTreeElement::Ptr makePrimesTreeElem(int level, const IntVector& vec, 
                                                 IntVector::const_iterator i) {
  if (level && i != vec.cend()) {
    PrimesTreeElement::Ptr e = std::make_shared<PrimesTreeElement>(PrimesValue(*i, false));
    if (level - 1) {
      e->left = makePrimesTreeElem(level-1, vec, ++i);
      e->right = makePrimesTreeElem(level-1, vec, ++i);
    }
    return e;
  } else {
    return nullptr; 
  }
}

static PrimesTreeElement::Ptr makePrimesTree(int level, 
                                             IntVector& vec) {
  return makePrimesTreeElem(level, vec, vec.cbegin());
}

static bool validatePrimesElem(PrimesTreeElement::Ptr e, PrimesMap& m) {
  if (e) {
    if ( m[e->v.first] != e->v.second ) {
      return false;
    }
    if (!validatePrimesElem(e->left, m) || !validatePrimesElem(e->right, m)) {
      return false;
    }
  }
  return true;
}

static void warmupTBB() {
  tbb::parallel_for(0, tbb::task_scheduler_init::default_num_threads(), [](int) {
    tbb::tick_count t0 = tbb::tick_count::now();
    while ((tbb::tick_count::now() - t0).seconds() < 0.01);
  });
}

int main() {
  const int levels = 14;
  const int N = std::pow(2, levels) - 1;

  PrimesMap m;
  auto vec = makePrimesValues(N, m);
  auto root = makePrimesTree(levels, vec);
  
  double parallel_time = 0.0;
  {
    warmupTBB();
    tbb::tick_count t0 = tbb::tick_count::now();
    fig_2_19(root);
    parallel_time = (tbb::tick_count::now() - t0).seconds();
    if (!validatePrimesElem(root, m)) {
      std::cerr << "Error: incorrect results!" << std::endl;
    }
  }
  std::cout << "parallel_time == " << parallel_time << " seconds" << std::endl;
  return 0;
}

