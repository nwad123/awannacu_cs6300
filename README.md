# AwannaCU - Several Parallel Line-of-Sight Calculator Implementations

![Visibility Map](./visibility_map_header.png)

## Prerequresites

- Required
  - CMake v3.26 or higher
  - C++17 compliant compiler
- Optional
  - CUDA support
  - OpenMP support
  - OpenMPI support

## Building

This project uses [CMake](./CMakeLists.txt) to build, and requires an internet connection as it pulls in
several libraries from the internet when building. When building several executables and shared libaries
are built. A description of the executable targets is given in the "Running" section.

### Unix-like Systems

A [Makefile](./Makefile) is provided for unix-like systems. To build the project simply type:

```sh
make
```

### Other Systems

For other systems you can just use CMake directly. Make a build directory inside of the AwannaCU directory.
Then change into the build directory and run:

```sh
cmake ..
cmake --build .
```

## Running

After the build is complete there will be several different executables. If you are on a unix system and used the provided
[`Makefile`](./Makefile) without modification, then the executables will be located in the `build` directory.

Here is a list of all the executables produced:

- `$BUILD_DIR/src/serial/serial`: A serial, single threaded solver. This solver uses a different implementation than
  the other executables, so the output is different between this solver and the others.
- `$BUILD_DIR/src/parallel_cpu/par_cpu`: A parallel (shared memory) solver.
- `$BUILD_DIR/src/parallel_gpu/par_gpu`: A gpu-based solver.
- `$BUILD_DIR/src/distributed_cpu/dist_cpu`: A distributed memory solver using OpenMPI.
- `$BUILD_DIR/src/distributed_gpu/dist_gpu`: A distributed memory solver using OpenMPI and CUDA.
- `$BUILD_DIR/lib/*`: several executables are produced for testing the shared library. Instead of running them individually
  it's recommended to just run `make test` on unix-like systems or `cd $BUILD_DIR; ctest` on other systems.

## Project Layout

### Directories

- [`lib`](./lib/): A shared static library that stores all of the code shared between
  all solvers. Input parsing, output printing, and basic datatypes are examples of
  data in this library. To use this library add this to your CMakeLists.txt:

  ```cmake
  target_link_libraries(<target> PRIVATE shared_lib)
  ```

  And in your C++ code make sure to include the ["core.hpp"](./lib/core.hpp) file. This
  includes all the headers from the shared library.

- [`src`](./src/): This is where the source for all of the individual implementations is.

  Each directory is named after the implementation it supports. The required packages for each
  individual solver are as follows:

  - serial: none
  - parallel_cpu: OpenMP
  - parallel_gpu: CUDA
  - distributed_cpu: OpenMPI
  - distributed_gpu: OpenMPI, CUDA

## Testing

The [compare.py](./compare.py) and [display.py](./display.py) scripts can be used to verify, view,
and compare results. To use these scripts `opencv` and `numpy` must be installed.

Additionally, the sample input [input.raw](./input.raw) was provided for us, and was used in testing
and verifying our implementations.
