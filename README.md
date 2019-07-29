# SourceCode

Source code of the examples provided in each chapter of the TBB book (2019).

Makefiles default to use of Intel C++ compiler, but specifying a CXX definition
on the command line can change the compilation to be with Microsoft (CXX=cl),
XCode (CXX=clang++), or Gnu C++ (CXX=g++).

TBB, which is needed of course, can be installed as part of the
Intel compiler install, or obtained separately.

The following commands worked for us prior to the release of the book:
on MacOS and Linux: make
on MacOS and Linux: make CXX=g++
on Windows: nmake /f Makefile.nmake
on Windows: nmake /f Makefile.nmake CXX=cl
(the latter requires use of vcvars64.bat (Visual Studio),
 and setting of INCLUDE environment variable to include
 the windows\tbb\include and windows\pstl\include
 directories in your TBB installation, and setting of
 LIB environment variable to search the
 tbb\lib\ia32_win\vc_mt and tbb\lib\intel64_win\vc_mt
 directories.)

---

Linux or Mac builds with: make
(clean up with: make clean)

Windows build with: nmake /f Makefile.nmake
(clean up with: nmake /f Makefile.nmake clean)

---

Ch18 and Ch19 (Chapters 18-19) require an OpenCL SDK to be installed (vendor of choice).
Khronos maintains a list at https://www.khronos.org/conformance/adopters/conformant-products/opencl
Intel's SDK is at https://software.intel.com/intel-opencl
NVidia's SDK is at https://developer.nvidia.com/opencl

Ch20 (Chapter 20) require that the hwloc library to be installed.
https://www.open-mpi.org/projects/hwloc/

Feel free to leave comments/suggestions/feedback on the git project website for the book.

Mike, Rafa, and James
