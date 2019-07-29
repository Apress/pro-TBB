# Requirements to run fig_18_11 examples

In addition to the TBB library, this chapter also requires OpenCL, should you want to run the different implementations of the heterogeneous triad vector operation. In Chapter 18, Figures 10-8 up to 10-11, explain the code included in source file `fig_18_11.cpp`, but here we also provide three other variations of the same code:

1. `fig_18_11.cpp`: Based on **cl2.hpp** OpenCL header file. **WITHOUT** Shared Virtual Memory
2. `fig_18_11-svm.cpp`: Based on **cl2.hpp** OpenCL header file. **WITH** Shared Virtual Memory (SVM)
3. `fig_18_11-CLheader`: Based on **cl.h** OpenCL header file. **WITHOUT** Shared Virtual Memory
4. `fig_18_11-svmCLheader.cpp`: Based on **cl.h** OpenCL header file. **WITH** Shared Virtual Memory (SVM)

The requirements to run these codes are:

1. Compile and run the code on a platform with an OpenCL capable GPU
2. Install the OpenCL runtime/driver and OpenCL SDK
3. Examples without SVM require at least OpenCL 1.2, but those leveraging SVM require at least OpenCL 2.0

OpenCL drivers should be installed depending on the platform at hand:

- [Intel drivers for Linux and Windows](https://software.intel.com/en-us/articles/opencl-drivers)
- [ARM drivers for Mali GPUs](https://developer.arm.com/tools-and-software/graphics-and-gaming/mali-drivers)
- [AMD, NVIDIA and others](https://www.khronos.org/opencl/resources)
- MacOS platforms (up to Mojave) come with OpenCL 1.2 preinstalled. OpenCL 2.0 is not available so `fig_18_11-svm.cpp` and `fig_18_11-svmCLheader.cpp` won't even compile.

The OpenCL SDK is already included in [Intel Studio 2019](https://software.intel.com/en-us/system-studio) (and you also get TBB, Intel compiler, VTune, etc. in the same package).

The C++ OpenCL header, `cl2.hpp`, is already included within the Intel Studio 2019 OpenCL installation, but it can be downloaded also from the [OpenCL C++ Bindings page](https://github.khronos.org/OpenCL-CLHPP/index.html). If you prefer the original C-based `cl.h` header file, try `fig_18_11-CLheader.cpp` and `fig_18_11-svmCLheader.cpp`.

Some systems provide different OpenCL platforms, and the GPU does not necessary lies on the first one. The four provided versions of `fig_18_11` example tackle this issue by traversing the different platforms and selecting the one with a GPU device.

Some informative links to get started with OpenCL:

* [https://software.intel.com/intel-opencl](https://software.intel.com/intel-opencl)
* [https://software.intel.com/en-us/articles/sdk-for-opencl-2019-gsg](https://software.intel.com/en-us/articles/sdk-for-opencl-2019-gsg)
