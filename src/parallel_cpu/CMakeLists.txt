# Get OpenMP
include(FindOpenMP)

# Set the executable sources
set(SRCS main.cpp parallel_cpu.cpp)

# Build the executable and link it
add_executable(par_cpu ${SRCS})
target_include_directories(par_cpu PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(par_cpu PRIVATE fmt::fmt)
target_link_libraries(par_cpu PRIVATE shared_lib)

if (OpenMP_FOUND)
    target_link_libraries(par_cpu PRIVATE OpenMP::OpenMP_CXX)
endif()

# Set compiler optimizations
#  -O3 for speed
#  -DNDEBUG for no debug functionality
#  -g for profiling symbols
set(INTERPROCEDURAL_OPTIMIZATION TRUE)
target_compile_options(par_cpu 
  PRIVATE -O3 -DNDEBUG -g
)

# add compiler warnings
target_project_warnings(par_cpu)
