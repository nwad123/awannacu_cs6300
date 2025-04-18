# Get MPI 
include(FindMPI)

if (MPI_CXX_FOUND)
    # Set the exectuable sources
    set(SRCS main.cpp distributed_cpu.cpp)

    add_executable(dist_cpu ${SRCS})
    target_include_directories(dist_cpu PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(dist_cpu PRIVATE fmt::fmt)
    target_link_libraries(dist_cpu PRIVATE shared_lib)
    target_link_libraries(dist_cpu PRIVATE MPI::MPI_CXX)

    # Set compiler optimizations
    #  -O3 for speed
    #  -DNDEBUG for no debug functionality
    #  -g for profiling symbols
    set(INTERPROCEDURAL_OPTIMIZATION TRUE)
    target_compile_options(dist_cpu 
        PRIVATE -O0 -DDEBUG -g3
    )

    target_project_warnings(dist_cpu)
endif()