# Copyright (C) 2019 Intel Corporation
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom
# the Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
# OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
# ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
# OR OTHER DEALINGS IN THE SOFTWARE.
#
# SPDX-License-Identifier: MIT

CXX = icc
CXXFLAGS = $(EXTRA_CXXFLAGS) -std=c++11 -O2
LIBS = -ltbb -lpthread
LIBS2 = $(LIBS) -ltbbmalloc

# \
!ifndef 0   # \
all clean:  _printmsg # \
# \
_printmsg:  # \
	@echo "for nmake, please specify /f Makefile.nmake"
# \
!else

.PHONY : all clean fig_B_08_parallel_do fig_B_11_parallel_for fig_B_12_parallel_for2 fig_B_15_parallel_for_each fig_B_17_parallel_invoke fig_B_21_parallel_pipeline fig_B_23_parallel_reduce fig_B_24_parallel_deterministic_reduce fig_B_27_parallel_scan fig_B_30_parallel_sort fig_B_33_pipeline fig_B_43_fg fig_B_48_fixed_pool fig_B_48_memory_pool fig_B_60_mutex fig_B_62_atomic fig_B_64_combinable fig_B_66_enumerable_thread_specific fig_B_68_flatten2d fig_B_69_tick_count fig_B_71_task_group fig_B_73_task_scheduler_init fig_B_78_exception_handling fig_B_81_parallel_STL hw_tst

all : fig_B_08_parallel_do fig_B_11_parallel_for fig_B_12_parallel_for2 fig_B_15_parallel_for_each fig_B_17_parallel_invoke fig_B_21_parallel_pipeline fig_B_23_parallel_reduce fig_B_24_parallel_deterministic_reduce fig_B_27_parallel_scan fig_B_30_parallel_sort fig_B_33_pipeline fig_B_43_fg fig_B_48_fixed_pool fig_B_48_memory_pool fig_B_60_mutex fig_B_62_atomic fig_B_64_combinable fig_B_66_enumerable_thread_specific fig_B_68_flatten2d fig_B_69_tick_count fig_B_71_task_group fig_B_73_task_scheduler_init fig_B_78_exception_handling fig_B_81_parallel_STL hw_tst

fig_B_08_parallel_do : 
	$(CXX) $(CXXFLAGS) -o fig_B_08_parallel_do fig_B_08_parallel_do.cpp $(LIBS)

fig_B_11_parallel_for : 
	$(CXX) $(CXXFLAGS) -o fig_B_11_parallel_for fig_B_11_parallel_for.cpp $(LIBS)

fig_B_12_parallel_for2 : 
	$(CXX) $(CXXFLAGS) -o fig_B_12_parallel_for2 fig_B_12_parallel_for2.cpp $(LIBS)

fig_B_15_parallel_for_each : 
	$(CXX) $(CXXFLAGS) -o fig_B_15_parallel_for_each fig_B_15_parallel_for_each.cpp $(LIBS)

fig_B_17_parallel_invoke : 
	$(CXX) $(CXXFLAGS) -o fig_B_17_parallel_invoke fig_B_17_parallel_invoke.cpp $(LIBS)

fig_B_21_parallel_pipeline : 
	$(CXX) $(CXXFLAGS) -o fig_B_21_parallel_pipeline fig_B_21_parallel_pipeline.cpp $(LIBS)

fig_B_23_parallel_reduce : 
	$(CXX) $(CXXFLAGS) -o fig_B_23_parallel_reduce fig_B_23_parallel_reduce.cpp $(LIBS)

fig_B_24_parallel_deterministic_reduce : 
	$(CXX) $(CXXFLAGS) -o fig_B_24_parallel_deterministic_reduce fig_B_24_parallel_deterministic_reduce.cpp $(LIBS)

fig_B_27_parallel_scan : 
	$(CXX) $(CXXFLAGS) -o fig_B_27_parallel_scan fig_B_27_parallel_scan.cpp $(LIBS)

fig_B_30_parallel_sort : 
	$(CXX) $(CXXFLAGS) -o fig_B_30_parallel_sort fig_B_30_parallel_sort.cpp $(LIBS)

fig_B_33_pipeline : 
	$(CXX) $(CXXFLAGS) -o fig_B_33_pipeline fig_B_33_pipeline.cpp $(LIBS)

fig_B_43_fg : 
	$(CXX) $(CXXFLAGS) -o fig_B_43_fg fig_B_43_fg.cpp $(LIBS)

fig_B_48_fixed_pool : 
	$(CXX) $(CXXFLAGS) -o fig_B_48_fixed_pool fig_B_48_fixed_pool.cpp $(LIBS2)

fig_B_48_memory_pool : 
	$(CXX) $(CXXFLAGS) -o fig_B_48_memory_pool fig_B_48_memory_pool.cpp $(LIBS2)

fig_B_60_mutex : 
	$(CXX) $(CXXFLAGS) -o fig_B_60_mutex fig_B_60_mutex.cpp $(LIBS)

fig_B_62_atomic : 
	$(CXX) $(CXXFLAGS) -o fig_B_62_atomic fig_B_62_atomic.cpp $(LIBS)

fig_B_64_combinable : 
	$(CXX) $(CXXFLAGS) -o fig_B_64_combinable fig_B_64_combinable.cpp $(LIBS)

fig_B_66_enumerable_thread_specific : 
	$(CXX) $(CXXFLAGS) -o fig_B_66_enumerable_thread_specific fig_B_66_enumerable_thread_specific.cpp $(LIBS)

fig_B_68_flatten2d : 
	$(CXX) $(CXXFLAGS) -o fig_B_68_flatten2d fig_B_68_flatten2d.cpp $(LIBS)

fig_B_69_tick_count : 
	$(CXX) $(CXXFLAGS) -o fig_B_69_tick_count fig_B_69_tick_count.cpp $(LIBS)

fig_B_71_task_group : 
	$(CXX) $(CXXFLAGS) -o fig_B_71_task_group fig_B_71_task_group.cpp $(LIBS)

fig_B_73_task_scheduler_init : 
	$(CXX) $(CXXFLAGS) -o fig_B_73_task_scheduler_init fig_B_73_task_scheduler_init.cpp $(LIBS)

fig_B_78_exception_handling : 
	$(CXX) $(CXXFLAGS) -o fig_B_78_exception_handling fig_B_78_exception_handling.cpp $(LIBS)

fig_B_81_parallel_STL : 
	$(CXX) $(CXXFLAGS) -o fig_B_81_parallel_STL fig_B_81_parallel_STL.cpp $(LIBS)

hw_tst : 
	$(CXX) $(CXXFLAGS) -o hw_tst hw_tst.cpp $(LIBS)

clean : 
	rm -f fig_B_08_parallel_do fig_B_11_parallel_for fig_B_12_parallel_for2 fig_B_15_parallel_for_each fig_B_17_parallel_invoke fig_B_21_parallel_pipeline fig_B_23_parallel_reduce fig_B_24_parallel_deterministic_reduce fig_B_27_parallel_scan fig_B_30_parallel_sort fig_B_33_pipeline fig_B_43_fg fig_B_48_fixed_pool fig_B_48_memory_pool fig_B_60_mutex fig_B_62_atomic fig_B_64_combinable fig_B_66_enumerable_thread_specific fig_B_68_flatten2d fig_B_69_tick_count fig_B_71_task_group fig_B_73_task_scheduler_init fig_B_78_exception_handling fig_B_81_parallel_STL hw_tst fig_B_08_parallel_do.o fig_B_11_parallel_for.o fig_B_12_parallel_for2.o fig_B_15_parallel_for_each.o fig_B_17_parallel_invoke.o fig_B_21_parallel_pipeline.o fig_B_23_parallel_reduce.o fig_B_24_parallel_deterministic_reduce.o fig_B_27_parallel_scan.o fig_B_30_parallel_sort.o fig_B_33_pipeline.o fig_B_43_fg.o fig_B_48_fixed_pool.o fig_B_48_memory_pool.o fig_B_60_mutex.o fig_B_62_atomic.o fig_B_64_combinable.o fig_B_66_enumerable_thread_specific.o fig_B_68_flatten2d.o fig_B_69_tick_count.o fig_B_71_task_group.o fig_B_73_task_scheduler_init.o fig_B_78_exception_handling.o fig_B_81_parallel_STL.o hw_tst.o *.bmp *.txt out*.png

# \
!endif
