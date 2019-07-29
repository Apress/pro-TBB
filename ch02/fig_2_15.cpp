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

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>
#include <tbb/tbb.h>

void fig_2_15(const std::vector<double>& altitude, std::vector<bool>& is_visible, double dx) {
  const int N = altitude.size();

  double max_angle = std::atan2(dx, altitude[0] - altitude[1]);
  double my_angle = 0.0;

  for (int i = 2; i < N; ++i ) {
    my_angle = std::atan2(i * dx, altitude[0] - altitude[i]);
    if (my_angle >= max_angle) {
      max_angle = my_angle;
    } else {
      is_visible[i] = false;
    }
  }
}

enum LOSInputType { ALT_FLAT=0, ALT_DECREASING, ALT_WAVE, ALT_PLATEAU };
static std::string LOSInputNames[] = {"flat", "decreasing", "wave", "plateau"};

static std::vector<double> makeAltitudeVector(int N, LOSInputType t) {
  std::vector<double> v(N);

  const int saw_period = N/10;
  double previous_value = N;
  double delta = -1;
  for (int i = 0; i < N; ++i) {
    switch (t) {
      case ALT_FLAT:
        if (i == 0)
          v[i] = N;
        else
          v[i] = N + 10;
        break;
      case ALT_DECREASING:
        if (i == 0) {
          v[i] = N;
          previous_value = -1;
        } else {
          v[i] = (++previous_value);
        }
        break;
      case ALT_WAVE:
        if (i == 0) {
          v[i] = previous_value = N;
          delta = -1;
        } else {
          v[i] = (previous_value += delta);
           if ( i % saw_period == 0 ) delta *= -1;
        }
        break;
      case ALT_PLATEAU:
        if ( i == 0 )
          v[i] = N;
        else if ( i < N/10 )
          v[i] = N / 2;
        else
          v[i] = N / 4;
        break;
    }
  }
  return v;
}

static void printVisibility(bool b, int f, int l) {
  std::cerr << f << " - " << l << " : ";
  if (b) {
    std::cerr << "visible" << std::endl;
  } else {
    std::cerr << "not visible" << std::endl;
  }
}

static bool checkVisibility(LOSInputType t, std::vector<bool>& is_visible) {
  const int N = is_visible.size();
  const int saw_period = N/10;
  int i = 0;
  
  switch (t) {
    case ALT_FLAT:
      if (!is_visible[0] || !is_visible[1]) {
        return false;
      }
      for (i = 2; i < N; ++i) {
        if (is_visible[i] == true) 
          return false;
      }
      return true;
      break;
    case ALT_DECREASING:
      for (i = 0; i < N; ++i) {
        if (is_visible[i] == false) 
            return false;
      }
      return true;
      break;
    case ALT_WAVE:
      for (i = 0; i < 2*saw_period; ++i) {
        if (is_visible[i] == false) 
          return false;
      }
      for (i = 2*saw_period; i < N; i += 2*saw_period) {
        if (is_visible[i] == false) 
          return false;
      }
      return true;
      break;
    case ALT_PLATEAU:
      // first part is visible
      i = 0;
      for ( ; i < saw_period; ++i) {
        if (is_visible[i] == false) 
          return false;
      }
      // second part is not visible
      while ( is_visible[i] == false && i < N) {
        ++i;
      }
      if (i <= saw_period || i == N)
        return false;
      // third part is visible
      for (; i < N; ++i) {
        if (is_visible[i] == false) 
          return false;
      }
      return true;
      break;
    default:
      return false;
  }
}

static void dumpLOS(std::vector<bool>& is_visible) {
  bool can_see_it = true;
  const int N = (int)is_visible.size();
  int first_index = 0;
  for (int i = 0; i < N; ++i) {
    bool b = is_visible[i];
    if (can_see_it != b) {
      printVisibility(can_see_it, first_index, i - 1);
      can_see_it = b;
      first_index = i;
    }
  }
  printVisibility(is_visible[N - 1], first_index, N - 1);
}

int main() {
  const int N = 100000000;
  const double dx = 1;

  auto input_types = { ALT_FLAT, ALT_DECREASING, ALT_WAVE, ALT_PLATEAU };
  std::vector<bool> is_visible(N, true);
  
  for ( auto i : input_types ) {
    std::vector<double> altitude = makeAltitudeVector(N, i);
    std::fill(is_visible.begin(), is_visible.end(), true);
    
    double serial_time = 0.0;
    {
      tbb::tick_count t0 = tbb::tick_count::now();
      fig_2_15(altitude, is_visible, 1.0);
      serial_time = (tbb::tick_count::now() - t0).seconds();
      dumpLOS(is_visible);
      if (checkVisibility(i, is_visible) != true) {
        std::cerr << "ERROR: visibility is not as expected!" << std::endl;
      }
    }
    std::cout << "serial_time for " << LOSInputNames[i] << " == " << serial_time << " seconds" << std::endl;
  }
  return 0;
}

