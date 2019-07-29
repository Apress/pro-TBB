# Requirements to run Chapter 20 examples

In addition to the TBB library, this chapter also requires the [hwloc](https://www.open-mpi.org/projects/hwloc/) library. In order to appreciate the performance gains when exploiting NUMA locality, the platform should contain at least two NUMA nodes. The library is supported in Linux, Windows and OS X, among other operating systems.

Follow these steps to install and use hwloc:

1. Download the library from the [download page](https://www.open-mpi.org/software/hwloc/v2.0/).
2. Install the library following the usual GNU-based procedure (more details [here](https://www.open-mpi.org/projects/hwloc/doc/v2.0.3/)):

    ```
    shell$ ./configure --prefix=<install_dir> && make && make install
    ```  

3. Adapt the Makefile to correctly point to the hwloc installation directory or set the HWLOC_PREFIX directory:

    ```
    export HWLOC_PREFIX=<install_dir>
    ```

4. On Linux, make sure the library will be found at runtime:

    ```
    export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${HWLOC_PREFIX}/lib
    ```
