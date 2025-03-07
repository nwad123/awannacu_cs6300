add_executable(par_gpu parallel_gpu_main.cpp)
target_link_libraries(par_gpu PRIVATE shared_lib)
target_link_libraries(par_gpu PRIVATE fmt::fmt)
