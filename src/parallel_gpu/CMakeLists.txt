set(SRCS main.cpp parallel_gpu.cpp)

add_executable(par_gpu ${SRCS})
target_include_directories(par_gpu PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(par_gpu PRIVATE fmt::fmt)
target_link_libraries(par_gpu PRIVATE shared_lib)

target_project_warnings(par_gpu)
