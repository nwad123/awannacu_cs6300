# AwannaCU - Several Parallel Line-of-Sight Calculator Implementations

TODO: write readme description.

## Prerequresites

- Required
  - CMake v3.25 or higher
  - C++20 compliant compiler
- Optional
  - CUDA support
  - TODO: Add MPI, OpenMP, etc. depending on what we use

## Building

### Unix-like Systems

```sh
make
```

### Other Systems

Make a build directory inside of the AwannaCU directory. Then change into the build directory and run:

```sh
cmake ..
cmake --build .
```

## Running The Command Line Application

After the build is complete the resulting command line application will be located at
`<build_directory>/cli/main`. If you are on a unix system and used the provided
[`Makefile`](./Makefile), then you can use the following command to start the command
line application.

```sh
./build/cli/main
```

## Project Layout

### Directories

- [`lib`](./lib/): A shared static library that stores all of the code shared between
  all solvers. Input parsing, the `Solver` concept, and basic datatypes are examples of
  data in this library. To use this library add this to your CMakeLists.txt:

  ```cmake
  target_link_libraries(<target> PRIVATE shared_lib)
  ```

  And in your C++ code make sure to include the ["core.hpp"](./lib/core.hpp) file. This
  includes all the headers from the shared library.

- [`src`](./src/): A shared static library containing all of the solver implementations.

  ```cmake
  target_link_libraries(<target> PRIVATE solvers)
  ```

  And in your C++ code make sure to include the ["solvers.hpp"](./src/solvers.hpp) file. This
  includes all the headers from the shared library.

- [`test`](./test/): Builds a test executable that can be run with `ctest` or directly.
