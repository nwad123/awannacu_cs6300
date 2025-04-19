# Source Code

This directory contains the source code for all of the different implementations of the line-of-sight
calculator. Each subdirectory contains the source code for a specific implementation.

## Subdirectories

- `serial`: Contains the source code for the serial, single-threaded implementation.
- `parallel_cpu`: Contains the source code for the parallel, shared-memory implementation using OpenMP.
- `parallel_gpu`: Contains the source code for the GPU-based implementation using CUDA.
- `distributed_cpu`: Contains the source code for the distributed-memory implementation using OpenMPI.
- `distributed_gpu`: Contains the source code for the distributed-memory implementation using OpenMPI and CUDA.

## Building

Each subdirectory contains its own `CMakeLists.txt` file. These files are used to build the executables
for each implementation. The top-level `CMakeLists.txt` file in the project root directory is responsible
for building all of the implementations.

## Running

Each subdirectory contains a `main.cpp` file that serves as the entry point for the executable. The
executables are built in the build directory and can be run from there. See the top-level `README.md`
file for more information on running the executables.
