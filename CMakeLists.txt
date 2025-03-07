cmake_minimum_required(VERSION 3.25)
project(AwannaCU)

# Enable C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

## Dependencies
# `GTest` for testing framework
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.16.0
)
FetchContent_MakeAvailable(googletest)

# `fmt` Library for better output
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG        11.1.4)
FetchContent_MakeAvailable(fmt)

# Subdirectory for the shared library
add_subdirectory(lib)

# Subdirectories for implementations
add_subdirectory(src/serial)
add_subdirectory(src/parallel_cpu)
add_subdirectory(src/parallel_gpu)
add_subdirectory(src/distributed_cpu)
add_subdirectory(src/distributed_gpu)

# Optional: Set build type to Release for performance
# set(CMAKE_BUILD_TYPE Release)

# Optional: Enable CUDA if needed
# find_package(CUDA REQUIRED)
# if(CUDA_FOUND)
#     enable_language(CUDA)
# endif()
