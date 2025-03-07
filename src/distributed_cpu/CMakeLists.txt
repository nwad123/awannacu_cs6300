add_executable(dist_cpu distributed_cpu_main.cpp)
target_link_libraries(dist_cpu PRIVATE shared_lib)
target_link_libraries(dist_cpu PRIVATE fmt::fmt)
