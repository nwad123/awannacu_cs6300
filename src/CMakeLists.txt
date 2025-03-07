set(SOLVERS
    "distributed_cpu/distributed_cpu.cpp"
    "distributed_gpu/distributed_gpu.cpp"
    "parallel_cpu/parallel_cpu.cpp"
    "parallel_gpu/parallel_gpu.cpp"
    "serial/serial.cpp")

add_library(solvers STATIC ${SOLVERS})
target_include_directories(solvers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(solvers PRIVATE fmt::fmt)
target_link_libraries(solvers PRIVATE shared_lib)
target_project_warnings(solvers)