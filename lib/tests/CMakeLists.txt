enable_testing()

include(GoogleTest)

function(add_test TESTNAME)
    add_executable(${TESTNAME} ${ARGN})
    target_link_libraries(${TESTNAME} gtest gtest_main shared_lib fmt::fmt)
    
    gtest_discover_tests(${TESTNAME})
endfunction()

add_test(read_input read_input.cpp)
