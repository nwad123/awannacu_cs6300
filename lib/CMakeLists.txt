add_library(shared_lib STATIC)
target_include_directories(shared_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(tests)
