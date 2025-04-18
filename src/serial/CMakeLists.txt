set(SRCS main.cpp serial.cpp)

add_executable(serial ${SRCS})
target_include_directories(serial PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(serial PRIVATE fmt::fmt)
target_link_libraries(serial PRIVATE shared_lib)

if (OpenMP_FOUND)
    target_link_libraries(serial PRIVATE OpenMP::OpenMP_CXX)
endif()

# Set compiler optimizations
#  -O3 for speed
#  -DNDEBUG for no debug functionality
#  -g for profiling symbols
set(INTERPROCEDURAL_OPTIMIZATION TRUE)
target_compile_options(serial 
  PRIVATE -O3 -DNDEBUG -g
)

target_project_warnings(serial)
