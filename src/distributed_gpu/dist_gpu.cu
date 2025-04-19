#include "dist_gpu.cuh"
#include <cmath>
#include <cuda_runtime.h>
#include <iostream>
#include <vector>
#include "fmt/core.h"

__global__ void calculate_visibility_kernel(
    const int16_t *height_map,
    unsigned int *visibility_map,
    int width,
    int height,
    int radius,
    int num_angles,
    int y_offset,
    float *ray_directions_x,
    float *ray_directions_y
)
{
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = (blockIdx.y * blockDim.y + threadIdx.y) + y_offset;

    // check bounds
    if (x >= width || y >= height) { return; }

    const int radius_squared = radius * radius;
    const int index = y * width + x;
    unsigned short current_height = height_map[index];

    // Start the count at this cell as 1 (the pixel itself is always visible)
    unsigned int visible_count = 1;

    // Cast rays in different directions
    for (int i = 0; i < num_angles; ++i) {
        float dx = ray_directions_x[i];
        float dy = ray_directions_y[i];

        int max_steps = radius;
        float max_angle_seen = -INFINITY;

        // Step size for more efficient ray traversal
        const float ray_length = sqrtf(dx * dx + dy * dy);
        const float step_x = dx / ray_length;
        const float step_y = dy / ray_length;

        float curr_x_f = x + 0.5f; // Start at center of pixel
        float curr_y_f = y + 0.5f;

        for (int step = 1; step <= max_steps; ++step) {
            // Move along the ray
            curr_x_f += step_x;
            curr_y_f += step_y;

            // Round to nearest pixel
            const int curr_x = __float2int_rn(curr_x_f);
            const int curr_y = __float2int_rn(curr_y_f);

            // Check bounds
            if (curr_x < 0 || curr_x >= width || curr_y < 0 || curr_y >= height) break;

            // Check if we've gone too far (outside the radius)
            int dist_squared = (curr_x - x) * (curr_x - x) + (curr_y - y) * (curr_y - y);
            if (dist_squared > radius_squared) break;

            // Get height at current position
            unsigned short point_height = height_map[curr_y * width + curr_x];

            // Calculate angle to determine visibility
            float distance = sqrtf(static_cast<float>(dist_squared));
            float height_diff = static_cast<float>(point_height) - static_cast<float>(current_height);
            float angle = height_diff / distance;

            // If the angle is greater than the maximum seen so far,
            // the pixel is visible
            if (angle > max_angle_seen) {
                max_angle_seen = angle;
                visible_count++;
            }
        }
    }

    // Store the visibility count
    visibility_map[index] = visible_count;
}

std::vector<unsigned int> calculate_visibility_cuda(
    const std::vector<int16_t> &height_map,
    size_t width,
    size_t height,
    int radius,
    int angle,
    const int start_y, 
    const int end_y, 
    const int my_rank
)
{
    // Calculate the width and height of the visibility map for this process
    const auto my_height = end_y - start_y;
    const auto my_y_offset = start_y;

    // Allocate host result for this process
    std::vector<unsigned int> visibility_map(width * my_height, ~0);

    // Number of discrete angles
    const int num_angles = std::abs(angle);
    // The distance between each angle in radians
    const double angle_step = 2 * M_PI / num_angles;

    // Allocate and initialize host ray directions
    std::vector<float> ray_directions_x(num_angles);
    std::vector<float> ray_directions_y(num_angles);

    // Precalculate the angle of the rays to be cast
    for (int i = 0; i < num_angles; ++i) {
        const double angle_ = i * angle_step;
        ray_directions_x[i] = static_cast<float>(std::cos(angle_) * radius);
        ray_directions_y[i] = static_cast<float>(std::sin(angle_) * radius);
    }

    // device memory
    int16_t *d_height_map = nullptr;
    unsigned int *d_visibility_map = nullptr;
    float *d_ray_directions_x = nullptr;
    float *d_ray_directions_y = nullptr;

    // size calculations
    size_t height_map_size = width * height * sizeof(int16_t);
    size_t visibility_map_size = width * my_height * sizeof(unsigned int);
    size_t ray_directions_size = num_angles * sizeof(float);

    cudaMalloc(&d_height_map, height_map_size);
    cudaMalloc(&d_visibility_map, visibility_map_size);
    cudaMalloc(&d_ray_directions_x, ray_directions_size);
    cudaMalloc(&d_ray_directions_y, ray_directions_size);

    // Copy data to device
    cudaMemcpy(d_height_map, height_map.data(), height_map_size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_ray_directions_x, ray_directions_x.data(), ray_directions_size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_ray_directions_y, ray_directions_y.data(), ray_directions_size, cudaMemcpyHostToDevice);

    // Set up grid and block dimensions
    dim3 block_size(16, 16);
    dim3 grid_size((width + block_size.x - 1) / block_size.x, (my_height + block_size.y - 1) / block_size.y);

    // Launch kernel
    std::cout << "Launching CUDA kernel with grid size: " << grid_size.x << "x" << grid_size.y
              << ", block size: " << block_size.x << "x" << block_size.y
              << " from process " << my_rank << std::endl;

    fmt::println("width {} height {} radius {} num_angles {} my_y_offset {}", width, height, radius, num_angles, my_y_offset);
    calculate_visibility_kernel<<<grid_size, block_size>>>(
        d_height_map, d_visibility_map, 
        width, height, radius, num_angles, my_y_offset, 
        d_ray_directions_x, d_ray_directions_y
    );

    // check for errors
    cudaError_t error = cudaGetLastError();
    if (error != cudaSuccess) {
        std::cerr << "CUDA kernel launch failed with error: " << cudaGetErrorString(error) << std::endl;
    }


    // wait for kernel to finish
    cudaDeviceSynchronize();

    // copy result back to host
    cudaMemcpy(visibility_map.data(), d_visibility_map, visibility_map_size, cudaMemcpyDeviceToHost);

    // Free device memory
    cudaFree(d_height_map);
    cudaFree(d_visibility_map);
    cudaFree(d_ray_directions_x);
    cudaFree(d_ray_directions_y);

    // print a sum of the visibility_map
    unsigned int sum = 0;
    for (unsigned int val : visibility_map) {
        sum += val;
    }
    std::cout << "Sum of visibility map on process " << my_rank << ": " << sum << std::endl;


    std::cout << "CUDA completed on process " << my_rank << std::endl;
    
    return visibility_map;
}
