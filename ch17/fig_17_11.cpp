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

#include <iostream>
#include <tbb/tbb.h>
#include <vector>

int N = 1 << 13;

using RType = tbb::blocked_range2d<int, int>;

double *setArray(int N, double *a) {
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      a[i*N + j] = i;
    }
  }
  return a;
}

void checkArray(int N, double *a) {
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      if (a[i*N + j] != j) {
        std::cout << "Transpose failed" << std::endl;
      }
    }
  }
}

void checkTranspose(int N, double *a) {
  return checkArray(N, a);
}

double *setBlock(const RType& r, double *a) {
  int ie = r.rows().end();
  int je = r.cols().end();
  for (int i = r.rows().begin(); i < ie; ++i) {
    for (int j = r.cols().begin(); j < je; ++j) {
      a[i*N + j] = i;
    }
  }
  return a;
}

double *setTransposedBlock(const RType& r, double *a) {
  int ie = r.rows().end();
  int je = r.cols().end();
  for (int j = r.cols().begin(); j < je; ++j) {
    for (int i = r.rows().begin(); i < ie; ++i) {
      a[j*N + i] = j;
    }
  }
  return a;
}

void checkTransposedBlock(const RType& r, double *a) {
  int ie = r.rows().end();
  int je = r.cols().end();
  for (int j = r.cols().begin(); j < je; ++j) {
    for (int i = r.rows().begin(); i < ie; ++i) {
      if (a[j*N + i] != i) {
        std::cout << "Transpose failed" << std::endl;
      }
    }
  }
}

double serialTranspose(int N, double *a, double *b) {
  tbb::tick_count t0 = tbb::tick_count::now();
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      b[j*N + i] = a[i*N + j];
    }
  }
  tbb::tick_count t1 = tbb::tick_count::now();
  return (t1 - t0).seconds();
}

void obliviousTranspose(int N, int ib, int ie, int jb, int je, double *a, double *b, int gs) {
  int ilen = ie - ib;
  int jlen = je - jb;
  if (ilen > gs || jlen > gs) {
    if (ilen > jlen) {
      int imid = (ib + ie) / 2;
      obliviousTranspose(N, ib, imid, jb, je, a, b, gs);
      obliviousTranspose(N, imid, ie, jb, je, a, b, gs);
    }
    else {
      int jmid = (jb + je) / 2;
      obliviousTranspose(N, ib, ie, jb, jmid, a, b, gs);
      obliviousTranspose(N, ib, ie, jmid, je, a, b, gs);
    }
  }
  else {
    for (int i = ib; i < ie; ++i) {
      for (int j = jb; j < je; ++j) {
        b[j*N + i] = a[i*N + j];
      }
    }
  }
}

double obliviousTranspose(int N, double *a, double *b, int gs) {
  tbb::tick_count t0 = tbb::tick_count::now();
  obliviousTranspose(N, 0, N, 0, N, a, b, gs);
  tbb::tick_count t1 = tbb::tick_count::now();
  return (t1 - t0).seconds();
}

template<typename P>
double pforBR2D(int N, double *a, double *b, int gs) {
  tbb::tick_count t0 = tbb::tick_count::now();
  tbb::parallel_for(tbb::blocked_range2d<int, int>{0, N, static_cast<size_t>(gs), 0, N, static_cast<size_t>(gs)},
    [N, a, b](const tbb::blocked_range2d<int, int>& r) {
      int ie = r.rows().end();
      int je = r.cols().end();
      for (int i = r.rows().begin(); i < ie; ++i) {
        for (int j = r.cols().begin(); j < je; ++j) {
          b[j*N + i] = a[i*N + j];
        }
      }
    }, P{}
  );
  tbb::tick_count t1 = tbb::tick_count::now();
  return (t1 - t0).seconds();
}

double serial3Transposes(int N, double *a[3], double *b[3]) {
  tbb::tick_count t0 = tbb::tick_count::now();
  for (int i = 0; i < 3; ++i) {
    setArray(N, a[i]);
    setArray(N, b[i]);
    serialTranspose(N, a[i], b[i]);
    checkTranspose(N, b[i]);
  }
  double total_time = (tbb::tick_count::now() - t0).seconds();

  // a double check
  for (int i = 0; i < 3; ++i) {
    checkTranspose(N, b[i]);
  }
  return total_time;
}

double oblivious3Transposes(int N, double *a[3], double *b[3], int cutoff) {
  tbb::tick_count t0 = tbb::tick_count::now();
  for (int i = 0; i < 3; ++i) {
    setArray(N, a[i]);
    setArray(N, b[i]);
    obliviousTranspose(N, a[i], b[i], cutoff);
    checkTranspose(N, b[i]);
  }
  double total_time = (tbb::tick_count::now() - t0).seconds();

  // a double check
  for (int i = 0; i < 3; ++i) {
    checkTranspose(N, b[i]);
  }
  return total_time;
}

double pfor3Transposes(int N, double *a[3], double *b[3], int gs) {
  tbb::tick_count t0 = tbb::tick_count::now();
  for (int i = 0; i < 3; ++i) {
    setArray(N, a[i]);
    setArray(N, b[i]);
    pforBR2D<tbb::simple_partitioner>(N, a[i], b[i], gs);
    checkTranspose(N, b[i]);
  }
  double total_time = (tbb::tick_count::now() - t0).seconds();

  // a double check
  for (int i = 0; i < 3; ++i) {
    checkTranspose(N, b[i]);
  }
  return total_time;
}

struct FGMsg {
  int N;
  double *a;
  double *b;
  FGMsg() : N(0), a(0), b(0) {}
  FGMsg(int _N, double *_a, double *_b) : N(_N), a(_a), b(_b) {}
};

double fig_17_9(int N, double *a[3], double *b[3]) {
  tbb::tick_count t0 = tbb::tick_count::now();
  tbb::flow::graph g;
  int i = 0;
  tbb::flow::source_node<FGMsg> initialize{g, [&](FGMsg& msg) -> bool {
    if (i < 3) {
      msg = {N, setArray(N, a[i]), setArray(N, b[i])};
      ++i;
      return true;
    } else {
      return false; 
    }
  }, false};
  tbb::flow::function_node<FGMsg, FGMsg> transpose{g, tbb::flow::unlimited,
    [](const FGMsg& msg) -> FGMsg {
      serialTranspose(msg.N, msg.a, msg.b);
      return msg;
  }};
  tbb::flow::function_node<FGMsg> check{g, tbb::flow::unlimited,
    [](const FGMsg& msg) -> FGMsg {
    checkArray(msg.N, msg.b);
    return msg;
  }};
  tbb::flow::make_edge(initialize, transpose);
  tbb::flow::make_edge(transpose, check);
  initialize.activate();
  g.wait_for_all();
  double total_time = (tbb::tick_count::now() - t0).seconds();

  // a double check
  for (int i = 0; i < 3; ++i) {
    checkTranspose(N, b[i]);
  }
  return total_time;
}

double fgObliviousTranspose(int N, double *a[3], double *b[3], int cutoff) {
  tbb::tick_count t0 = tbb::tick_count::now();
  tbb::flow::graph g;
  int i = 0;
  tbb::flow::source_node<FGMsg> initialize{g, [&](FGMsg& msg) -> bool {
    if (i < 3) {
      msg = {N, setArray(N, a[i]), setArray(N, b[i])};
      ++i;
      return true;
    }
    else {
      return false;
    }
  }, false};
  tbb::flow::function_node<FGMsg, FGMsg> transpose{g, tbb::flow::unlimited,
    [cutoff](const FGMsg& msg) -> FGMsg {
    obliviousTranspose(msg.N, msg.a, msg.b, cutoff);
    return msg;
  }};
  tbb::flow::function_node<FGMsg> check{g, tbb::flow::unlimited,
    [](const FGMsg& msg) -> FGMsg {
    checkArray(msg.N, msg.b);
    return msg;
  }};
  tbb::flow::make_edge(initialize, transpose);
  tbb::flow::make_edge(transpose, check);
  initialize.activate();
  g.wait_for_all();

  double total_time = (tbb::tick_count::now() - t0).seconds();
  // a double check
  for (int i = 0; i < 3; ++i) {
    checkTranspose(N, b[i]);
  }
  return total_time;
}

double fgPforTranspose(int N, double *a[3], double *b[3], int gs) {
  tbb::tick_count t0 = tbb::tick_count::now();
  tbb::flow::graph g;
  int i = 0;
  tbb::flow::source_node<FGMsg> initialize{g, [&](FGMsg& msg) -> bool {
    if (i < 3) {
      msg = {N, setArray(N, a[i]), setArray(N, b[i])};
      ++i;
      return true;
    }
    else {
      return false;
    }
  }, false};
  tbb::flow::function_node<FGMsg, FGMsg> transpose{g, tbb::flow::unlimited,
    [gs](const FGMsg& msg) -> FGMsg {
    int N = msg.N;
    double *a = msg.a;
    double *b = msg.b;
    tbb::parallel_for(tbb::blocked_range2d<int, int>(0, N, gs, 0, N, gs),
      [N, a, b](const tbb::blocked_range2d<int, int>& r) {
      int ie = r.rows().end();
      int je = r.cols().end();
      for (int i = r.rows().begin(); i < ie; ++i) {
        for (int j = r.cols().begin(); j < je; ++j) {
          b[j*N + i] = a[i*N + j];
        }
      }
    }, tbb::simple_partitioner());
    return msg;
  }};
  tbb::flow::function_node<FGMsg> check{g, tbb::flow::unlimited,
    [](const FGMsg& msg) -> FGMsg {
    checkArray(msg.N, msg.b);
    return msg;
  }};
  tbb::flow::make_edge(initialize, transpose);
  tbb::flow::make_edge(transpose, check);
  initialize.activate();
  g.wait_for_all();

  double total_time = (tbb::tick_count::now() - t0).seconds();
  // a double check
  for (int i = 0; i < 3; ++i) {
    checkTranspose(N, b[i]);
  }
  return total_time;
}

struct FGTiledMsg {
  int N;
  double *a;
  double *b;
  RType r;
  FGTiledMsg() : N(0), a(0), b(0), r(0, 0, 0, 0, 0, 0) {}
  FGTiledMsg(int _N, double *_a, double *_b, const RType& _r) 
    : N(_N), a(_a), b(_b), r(_r) {}
};

double fig_17_10(int N, double *a[3], double *b[3], int gs) {
  tbb::tick_count t0 = tbb::tick_count::now();
  tbb::flow::graph g;
  int i = 0;

  std::vector<RType> stack;
  stack.push_back(RType(0, N, gs, 0, N, gs));
  tbb::flow::source_node<FGTiledMsg> initialize{g, [&](FGTiledMsg& msg) -> bool {
    if (i < 3) {
      if (stack.empty()) {
        ++i;
        if (i == 3) return false;
        stack.push_back(RType(0, N, gs, 0, N, gs));
      }
      RType r = stack.back();
      stack.pop_back();
      while (r.is_divisible()) {
        RType rhs(r, tbb::split());
        stack.push_back(rhs);
      }
      msg = {N, setBlock(r, a[i]), setTransposedBlock(r, b[i]), r};
      return true;
    } else {
      return false;
    }
  }, false};
  tbb::flow::function_node<FGTiledMsg, FGTiledMsg> transpose{g, tbb::flow::unlimited,
    [](const FGTiledMsg& msg) {
    double *a = msg.a;
    double *b = msg.b;
    int N = msg.N;
    int ie = msg.r.rows().end();
    int je = msg.r.cols().end();
    for (int i = msg.r.rows().begin(); i < ie; ++i) {
      for (int j = msg.r.cols().begin(); j < je; ++j) {
        b[j*N + i] = a[i*N + j];
      }
    }
    return msg;
  }};
  tbb::flow::function_node<FGTiledMsg> check{g, tbb::flow::unlimited,
    [](const FGTiledMsg& msg) {
    checkTransposedBlock(msg.r, msg.b);
  }};
  tbb::flow::make_edge(initialize, transpose);
  tbb::flow::make_edge(transpose, check);
  initialize.activate();
  g.wait_for_all();

  double total_time = (tbb::tick_count::now() - t0).seconds();
  // a double check
  for (int i = 0; i < 3; ++i) {
    checkTranspose(N, b[i]);
  }
  return total_time;
}

double fgTiledObliviousTranspose(int N, double *a[3], double *b[3], int gs, int cutoff) {
  tbb::tick_count t0 = tbb::tick_count::now();
  tbb::flow::graph g;
  int i = 0;

  std::vector<RType> stack;
  stack.push_back(RType(0, N, gs, 0, N, gs));
  tbb::flow::source_node<FGTiledMsg> initialize{g, [&](FGTiledMsg& msg) -> bool {
    if (i < 3) {
      if (stack.empty()) {
        ++i;
        if (i == 3) return false;
        stack.push_back(RType(0, N, gs, 0, N, gs));
      }
      RType r = stack.back();
      stack.pop_back();
      while (r.is_divisible()) {
        RType rhs(r, tbb::split());
        stack.push_back(rhs);
      }
      msg = {N, setBlock(r, a[i]), setTransposedBlock(r, b[i]), r};
      return true;
    } else {
      return false;
    }
  }, false};
  tbb::flow::function_node<FGTiledMsg, FGTiledMsg> transpose{g, tbb::flow::unlimited,
    [cutoff](const FGTiledMsg& msg) {
    obliviousTranspose(msg.N, msg.r.rows().begin(), msg.r.rows().end(), 
                       msg.r.cols().begin(), msg.r.cols().end(), msg.a, msg.b, cutoff);
    return msg;
  }};
  tbb::flow::function_node<FGTiledMsg> check{g, tbb::flow::unlimited,
    [](const FGTiledMsg& msg) {
    checkTransposedBlock(msg.r, msg.b);
  }};
  tbb::flow::make_edge(initialize, transpose);
  tbb::flow::make_edge(transpose, check);
  initialize.activate();
  g.wait_for_all();

  double total_time = (tbb::tick_count::now() - t0).seconds();
  // a double check
  for (int i = 0; i < 3; ++i) {
    checkTranspose(N, b[i]);
  }
  return total_time;
}

double fgTiledPforTranspose(int N, double *a[3], double *b[3], int gs) {
  tbb::tick_count t0 = tbb::tick_count::now();
  tbb::flow::graph g;
  int i = 0;

  std::vector<RType> stack;
  stack.push_back(RType(0, N, gs, 0, N, gs));
  tbb::flow::source_node<FGTiledMsg> initialize{g, [&](FGTiledMsg& msg) -> bool {
    if (i < 3) {
      if (stack.empty()) {
        ++i;
        if (i == 3) return false;
        stack.push_back(RType(0, N, gs, 0, N, gs));
      }
      RType r = stack.back();
      stack.pop_back();
      while (r.is_divisible()) {
        RType rhs(r, tbb::split());
        stack.push_back(rhs);
      }
      msg = {N, setBlock(r, a[i]), setTransposedBlock(r, b[i]), r};
      return true;
    } else {
      return false;
    }
  }, false};
  tbb::flow::function_node<FGTiledMsg, FGTiledMsg> transpose{g, tbb::flow::unlimited,
    [](const FGTiledMsg& msg) {
    double *a = msg.a;
    double *b = msg.b;
    int N = msg.N;
    int msg_ib = msg.r.rows().begin();
    int msg_ie = msg.r.rows().end();
    int msg_jb = msg.r.cols().begin();
    int msg_je = msg.r.cols().end();
    tbb::parallel_for(tbb::blocked_range2d<int, int>(msg_ib, msg_ie, 1, msg_jb, msg_je, 1),
      [N, a, b](const tbb::blocked_range2d<int, int>& r) {
      int ie = r.rows().end();
      int je = r.cols().end();
      for (int i = r.rows().begin(); i < ie; ++i) {
        for (int j = r.cols().begin(); j < je; ++j) {
          b[j*N + i] = a[i*N + j];
        }
      }
    }, tbb::auto_partitioner());
    return msg;
  }};
  tbb::flow::function_node<FGTiledMsg> check{g, tbb::flow::unlimited,
    [](const FGTiledMsg& msg) {
    checkTransposedBlock(msg.r, msg.b);
  }};
  tbb::flow::make_edge(initialize, transpose);
  tbb::flow::make_edge(transpose, check);
  initialize.activate();
  g.wait_for_all();

  double total_time = (tbb::tick_count::now() - t0).seconds();
  // a double check
  for (int i = 0; i < 3; ++i) {
    checkTranspose(N, b[i]);
  }
  return total_time;
}

int main() {
  double *a[3];
  double *b [3];

  for (int i = 0; i < 3; ++i) {
    a[i] = new double[N*N];
    b[i] = new double[N*N];
  }

  serial3Transposes(N, a, b);
  double ts = serial3Transposes(N, a, b);

  oblivious3Transposes(N, a, b, 8);
  double to = oblivious3Transposes(N, a, b, 8);

  const int gs = 32;
  const int cutoff = 8;
  pfor3Transposes(N, a, b, gs);
  double t_pfor = pfor3Transposes(N, a, b, gs);

  fig_17_9(N, a, b);
  double t_fg_s = fig_17_9(N, a, b);

  fgObliviousTranspose(N, a, b, cutoff);
  double t_fg_o = fgObliviousTranspose(N, a, b, cutoff);

  fgPforTranspose(N, a, b, gs);
  double t_fg_pfor = fgPforTranspose(N, a, b, gs);

  fig_17_10(N, a, b, gs);
  double t_fg_tiled = fig_17_10(N, a, b, gs);

  fgTiledPforTranspose(N, a, b, gs);
  double t_fg_tiled_pfor = fgTiledPforTranspose(N, a, b, gs);

  std::cout << "               , serial, oblivious, pfor-br2d, flow graph (fig_17_9)"
            << ", flow graph + oblivious, flow graph + pfor-br2d"
            << ", tiled flow graph (fig_17_10), tiled flow graph + pfor-br2d" << std::endl; 
  std::cout << "time (seconds), " << ts << ", " << to << ", " << t_pfor
            << ", " << t_fg_s << ", " << t_fg_o  << ", " << t_fg_pfor
            << ", " << t_fg_tiled << ", " << t_fg_tiled_pfor << std::endl;
  std::cout << "speedup,       " << ts/ts << ", " << ts/to << ", " << ts/t_pfor
            << ", " << ts/t_fg_s << ", " << ts/t_fg_o  << ", " << ts/t_fg_pfor
            << ", " << ts/t_fg_tiled << ", " << ts/t_fg_tiled_pfor << std::endl;

  return 0;
}

