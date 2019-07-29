UNAME := $(shell uname)

ifeq ($(UNAME),Linux)
#Compile OpenCL on Linux
framework = -lOpenCL
endif

ifeq ($(UNAME),Darwin)
#Compile OpenCL on Mac
framework = -framework OpenCL
endif
CFLAGS=
CXX = g++ # Options: g++ for gcc, icpc for icc.
prog1 = Hello_OCLNode
prog2 = Hello_OCLNode-gpu
prog3 = Hello_ranges
prog4 = Hello_ranges2
prog5 = TwoGPUs
prog6 = devices
prog7 = ordering-wrong
prog8 = ordering-ok


$(prog1): $(prog1).cpp
	$(CXX) $^ -o $@ -std=c++11 -ltbb -w -O3 $(framework)
$(prog2): $(prog2).cpp
	$(CXX) $^ -o $@ -std=c++14 -ltbb -w -O3 $(framework)
$(prog3): $(prog3).cpp
	$(CXX) $^ -o $@ -std=c++14 -ltbb -w -O3 $(framework)

#Use make Hello_ranges2 -e CFLAGS=-DVERBOSE to activate VERBOSE
$(prog4): $(prog4).cpp
	$(CXX) $^ -o $@ -std=c++14 -ltbb -w -O3 $(framework) $(CFLAGS)
$(prog5): $(prog5).cpp
	$(CXX) $^ -o $@ -std=c++14 -ltbb -w -O3 $(framework)
$(prog6): $(prog6).cpp
	$(CXX) $^ -o $@ -std=c++11 -ltbb -w -O3 $(framework)
$(prog7): $(prog7).cpp
	$(CXX) $^ -o $@ -std=c++14 -ltbb -w -O3 $(framework)
$(prog8): $(prog8).cpp
	$(CXX) $^ -o $@ -std=c++14 -ltbb -w -O3 $(framework)

all : Hello_OCLNode

clean :
	rm -f $(prog1) $(prog2) $(prog3) $(prog4) $(prog5) $(prog6) $(prog7) $(prog8)
