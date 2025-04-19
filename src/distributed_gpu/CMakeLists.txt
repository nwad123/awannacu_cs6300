# Get MPI 
include(FindMPI)

if(CUDA_FOUND AND MPI_CXX_FOUND)
    # Find Cuda Toolkit
    include(FindCUDAToolkit)
    
    # Enable Cuda 
    enable_language(CUDA)
    
    # Set main sources up
    set(SRCS main.cpp args.cpp)

    # Set cuda sources up
    set(CUDA_SRCS dist_gpu.cu)

    # Compile the cuda kernel
    add_library(dist_gpu_kernel ${CUDA_SRCS})
    set_target_properties(dist_gpu_kernel PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
    target_link_libraries(dist_gpu_kernel PRIVATE fmt::fmt)
    
    # Compile the main executable
    add_executable(dist_gpu ${SRCS})
    target_include_directories(dist_gpu PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

    # Link the kernel and other libraries in
    target_link_libraries(dist_gpu PRIVATE dist_gpu_kernel)
    target_link_libraries(dist_gpu PRIVATE CUDA::cudart)
    target_link_libraries(dist_gpu PRIVATE MPI::MPI_CXX)
    target_link_libraries(dist_gpu PRIVATE fmt::fmt)
    target_link_libraries(dist_gpu PRIVATE shared_lib)
    
    target_project_warnings(dist_gpu)
endif()
