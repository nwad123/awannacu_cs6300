cmake_minimum_required(VERSION 3.25)
project(AwannaCU)

# Enable C++20
set(CMAKE_CXX_STANDARD 20)
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

# Turn on compile commands for IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add detailed compiler warnings
include(cmake/CompilerWarnings.cmake)

# Include the solver headers
include_directories(include)

# Subdirectory for the shared library
add_subdirectory(lib)

# Subdirectories for executable
add_subdirectory(src)

# Optional: Set build type to Release for performance
# set(CMAKE_BUILD_TYPE Release)

# Optional: Enable CUDA if needed
# find_package(CUDA REQUIRED)
# if(CUDA_FOUND)
#     enable_language(CUDA)
# endif()
