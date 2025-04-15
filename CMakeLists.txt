cmake_minimum_required(VERSION 3.26)
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

# Turn on compile commands for IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add detailed compiler warnings
include(cmake/CompilerWarnings.cmake)

# Include the `add_test` function and enable testing functionality
enable_testing()
include(GoogleTest)
include(cmake/TestFunction.cmake)

# Include the solver headers
include_directories(include)

# Subdirectory for all of the shared functionality
add_subdirectory(lib)

# Subdirectory for solver implementations
add_subdirectory(src)
