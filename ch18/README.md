# Requirements to run fig_18_11 examples


In addition to the TBB library, this chapter also requires OpenCL, should you want to run the different implementations of the heterogeneous triad vector operation. In Chapter 18, Figures 10-8 up to 10-11, explain the code included in source file `fig_18_11.cpp`, but here we also provide three other variations of the same code:

1. `fig_18_11.cpp`: Based on **opencl.hpp** OpenCL C++ header file. **WITHOUT** Shared Virtual Memory
2. `fig_18_11-svm.cpp`: Based on **opencl.hpp** OpenCL C++ header file. **WITH** Shared Virtual Memory (SVM)
3. `fig_18_11-CLheader`: Based on **cl.h** OpenCL header file. **WITHOUT** Shared Virtual Memory
4. `fig_18_11-svmCLheader.cpp`: Based on **cl.h** OpenCL header file. **WITH** Shared Virtual Memory (SVM)

**NOTE**: in oneAPI, C++ with SYCL is a more productive alternative to exploit GPU than OpenCL. The C++ with SYCL implementation of fig_18_11 can be found in the [oneAPI Samples repository](https://github.com/oneapi-src/oneAPI-samples/blob/master/Libraries/oneTBB/tbb-async-sycl/src/tbb-async-sycl.cpp)

The requirements to run these codes are:

1. Compile and run the code on a platform with an OpenCL capable integrated GPU
2. Install the OpenCL runtime/driver and OpenCL SDK. oneAPI installations require OpenCL so if oneAPI is already installed, OpenCL should be available.
3. Examples without SVM require at least OpenCL 1.2, but those leveraging SVM require at least OpenCL 2.0 and SVM FINE GRAIN BUFFER CAPABILITY

OpenCL drivers should be installed depending on the platform at hand:

- [Intel drivers for Linux and Windows](https://software.intel.com/en-us/articles/opencl-drivers)
- [ARM drivers for Mali GPUs](https://developer.arm.com/tools-and-software/graphics-and-gaming/mali-drivers)
- [AMD, NVIDIA and others](https://www.khronos.org/opencl/resources)
- MacOS platforms (up to Mojave) came with OpenCL 1.2 preinstalled. OpenCL 2.0 is not available so `fig_18_11-svm.cpp` and `fig_18_11-svmCLheader.cpp` won't even compile.

The C OpenCL header, `cl.h`, is already included within the oneAPI installation. The C++ OpenCL header, `opencl.hpp` can be downloaded from the [OpenCL C++ Bindings page](https://github.com/KhronosGroup/OpenCL-CLHPP). If you prefer the original C-based `cl.h` header file, try `fig_18_11-CLheader.cpp` and `fig_18_11-svmCLheader.cpp`.

Some systems provide different OpenCL platforms, and the GPU does not necessary lies on the first one. The four provided versions of `fig_18_11` example tackle this issue by traversing the different platforms and selecting the one with a GPU device.

Some informative links to get started with OpenCL:

* [Intel Tools for OpenCL applications](https://www.intel.com/content/www/us/en/developer/articles/tool/tools-for-opencl-applications.html?wapkw=opencl)
* [OpenCL Runtimes for Intel Processors](https://www.intel.com/content/www/us/en/developer/articles/tool/opencl-drivers.html)
