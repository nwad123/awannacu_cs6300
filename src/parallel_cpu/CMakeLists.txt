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

set(INTERPROCEDURAL_OPTIMIZATION TRUE)
target_compile_options(par_cpu 
  PRIVATE -O1 -DDEBUG -g3
)

# add compiler warnings
target_project_warnings(par_cpu)
