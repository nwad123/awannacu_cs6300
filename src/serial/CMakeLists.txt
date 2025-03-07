add_executable(serial serial_main.cpp)
target_link_libraries(serial PRIVATE shared_lib)
target_link_libraries(serial PRIVATE fmt::fmt)
