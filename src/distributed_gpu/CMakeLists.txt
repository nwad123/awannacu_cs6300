if(CUDA_FOUND)
    # Find Cuda Toolkit
    include(FindCUDAToolkit)
    
    # Enable Cuda 
    enable_language(CUDA)
    
    # Set sources up
    set(SRCS main.cpp distributed_gpu.cpp)
    
    add_executable(dist_gpu ${SRCS})
    target_include_directories(dist_gpu PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(dist_gpu PRIVATE fmt::fmt)
    target_link_libraries(dist_gpu PRIVATE shared_lib)
    
    target_project_warnings(dist_gpu)
endif()
