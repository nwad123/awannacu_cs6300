add_executable(par_cpu parallel_cpu_main.cpp)
target_link_libraries(par_cpu PRIVATE shared_lib)
target_link_libraries(par_cpu PRIVATE fmt::fmt)
