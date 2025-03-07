enable_testing()

add_executable(shared_lib_tests shared_lib_tests.cpp)
target_link_libraries(shared_lib_tests PUBLIC shared_lib)
target_link_libraries(shared_lib_tests PUBLIC fmt::fmt)
