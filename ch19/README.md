# Requirements to run Chapter 19 examples

In addition to the TBB library, this chapter also requires OpenCL.

The requirements to run these codes are:

1. Compile and run the code on a platform with an OpenCL capable device.
2. Install the OpenCL runtime/driver and OpenCL SDK
3. Examples `fig_19_08.cpp`, `fig_19_22.cpp` and `fig_19_24.cpp` require C++14

Some systems provide different OpenCL platforms, and the GPU does not necessary lies on the first one. However, due to TBB OpenCL node limitations, the devices that can be used with the current OpenCL factory should be in the **first OpenCL platform**. For some examples a GPU should be available, this is, the first OpenCL platform should include a GPU device. More precisely, these are the assumptions that were made for some of the examples:

* `fig_19_08.cpp` and `fig_19_09.cpp`: These two examples assume that the GPU is the second device of the first OpenCL platform. Change line 37 of the source file (`tbb::flow::opencl_device d = *(++f.devices().cbegin()); //LOOK!`) if this condition does not hold. To avoid a null pointer dereference if there aren't 2 devices in the first platform, the code defaults to the first one.
* `fig_19_16.cpp`: This example executes the heterogeneous Triad vector computation (part of the iterations computed on the GPU and the other part on the CPU). If there is no GPU available in the first platforms, it runs the GPU computation on the first OpenCL available device, with may result in sub-obtimal performance.
* `fig_19_22.cpp`: This example assumes that there are 3 OpenCL devices in the first platform, and that two GPUs are the 2nd and 3rd ones. If this is not the case, the code defaults to the first device. You can also adjust lines 40 (`auto d = *(f.devices().begin() + 1);`) and 51 (`auto d = *(f.devices().begin() + 2);`) of the source code accordingly.
* `fig_19_24.cpp`: This code is more illustrative if there are at least two OpenCL devices, ideally three, but it will work with only one as well.

OpenCL drivers should be installed depending on the platform at hand:

- [Intel drivers for Linux and Windows](https://software.intel.com/en-us/articles/opencl-drivers)
- [ARM drivers for Mali GPUs](https://developer.arm.com/tools-and-software/graphics-and-gaming/mali-drivers)
- [AMD, NVIDIA and others](https://www.khronos.org/opencl/resources)
- MacOS platforms (up to Mojave) comes with OpenCL 1.2 preinstalled. All examples have been tested and run on a MacBook Pro "late 2016".

The OpenCL SDK is already included in [Intel Studio 2019](https://software.intel.com/en-us/system-studio) (and you also get TBB, Intel compiler, VTune, etc. in the same package).


Some informative links to get started with OpenCL:

* [https://software.intel.com/intel-opencl](https://software.intel.com/intel-opencl)
* [https://software.intel.com/en-us/articles/sdk-for-opencl-2019-gsg](https://software.intel.com/en-us/articles/sdk-for-opencl-2019-gsg)
