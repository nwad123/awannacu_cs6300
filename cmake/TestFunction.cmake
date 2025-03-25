function(new_test TESTNAME MAIN)
    add_executable(${TESTNAME} ${MAIN})
    target_link_libraries(${TESTNAME} PUBLIC gtest gtest_main)
    target_link_libraries(${TESTNAME} PUBLIC ${ARGN})
    target_project_warnings(${TESTNAME})
    
    gtest_discover_tests(${TESTNAME})
endfunction()
