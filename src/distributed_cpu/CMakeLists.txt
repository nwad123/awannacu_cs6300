# Get MPI 
include(FindMPI)

# Set the exectuable sources
set(SRCS mpi_cpu_awcu.cpp)

add_executable(dist_cpu ${SRCS})
target_include_directories(dist_cpu PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(dist_cpu PRIVATE fmt::fmt)
target_link_libraries(dist_cpu PRIVATE shared_lib)

if (MPI_FOUND)
    target_link_libraries(dist_cpu PRIVATE MPI::MPI_CXX)
endif()

target_project_warnings(dist_cpu)
