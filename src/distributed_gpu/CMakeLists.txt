add_executable(dist_gpu distributed_gpu_main.cpp)
target_link_libraries(dist_gpu PRIVATE shared_lib)
target_link_libraries(dist_gpu PRIVATE fmt::fmt)
