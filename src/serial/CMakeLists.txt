set(SRCS main.cpp serial.cpp)

add_executable(serial ${SRCS})
target_include_directories(serial PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(serial PRIVATE fmt::fmt)
target_link_libraries(serial PRIVATE shared_lib)

if (OpenMP_FOUND)
    target_link_libraries(serial PRIVATE OpenMP::OpenMP_CXX)
endif()

target_project_warnings(serial )
