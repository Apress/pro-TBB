UNAME := $(shell uname)

ifeq ($(UNAME),Linux)
#Compile OpenCL on Linux
framework = -lOpenCL
endif

ifeq ($(UNAME),Darwin)
#Compile OpenCL on Mac
framework = -framework OpenCL
endif

CXX = g++ # Options: g++ for gcc, icpc for icc.
prog1 = triad_oclNode
prog2 = triad_oclPrecomp
prog3 = triad_oclNode-subbuff
prog4 = triad_oclNode-token


$(prog1): $(prog1).cpp
	$(CXX) $^ -o $@ -std=c++11 -ltbb -w -O3 $(framework)
$(prog2): $(prog2).cpp
	$(CXX) $^ -o $@ -std=c++11 -ltbb -w -O3 $(framework)
$(prog3): $(prog3).cpp
	$(CXX) $^ -o $@ -std=c++14 -ltbb -w -O3 $(framework)
$(prog4): $(prog4).cpp
	$(CXX) $^ -o $@ -std=c++11 -ltbb -w -O3 $(framework)

all : Hello_OCLNode

clean :
	rm -f $(prog1) $(prog2) $(prog3) $(prog4)
