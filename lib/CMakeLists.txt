add_library(shared_lib STATIC core.cpp)
target_link_libraries(shared_lib PRIVATE fmt::fmt)
target_include_directories(shared_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_project_warnings(shared_lib)

add_subdirectory(tests)
