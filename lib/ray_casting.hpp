#include <vector>
#include <cmath>
#include <limits>
#include <cstdint> // For int16_t, uint64_t
#include <utility> // For std::pair

static constexpr inline auto single_pixel_visiblity(
    const size_t x,                        
    const size_t y,                        
    const size_t width,                    
    const size_t height,                   
    const int radius,                      
    const int radius_squared,              
    const std::vector<int16_t>& height_map,
    const std::vector<std::pair<float, float>>& ray_directions) -> int 
{
    // Get the height of the current pixel
    const unsigned short current_height = height_map[y * width + x];

    // Start the count at this cell as 1 (the pixel itself is always visible)
    unsigned int visible_count = 1;

    // Cast rays in different directions
    for (const auto& [dx, dy] : ray_directions) {
        // Use a more efficient ray casting approach
        // Start from the center and move outward
        const int max_steps = radius; // Maximum steps roughly corresponds to radius
        float max_angle_seen = -std::numeric_limits<float>::infinity();

        // Step size for ray traversal
        // Avoid division by zero if dx and dy are both zero (though unlikely with precalculated directions)
        const float ray_length = std::sqrt(dx * dx + dy * dy);
        const float step_x = dx / ray_length;
        const float step_y = dy / ray_length;

        float curr_x_f = static_cast<float>(x) + 0.5f;  // Start at center of pixel
        float curr_y_f = static_cast<float>(y) + 0.5f;

        for (int step = 1; step <= max_steps; ++step) {
            // Move along the ray by one unit distance
            curr_x_f += step_x;
            curr_y_f += step_y;

            // Round to nearest pixel coordinates
            const int curr_x = static_cast<int>(std::round(curr_x_f));
            const int curr_y = static_cast<int>(std::round(curr_y_f));

            // --- Bounds and Radius Checks ---
            // Check bounds first (more likely to fail early for edge pixels)
            if (curr_x < 0 || curr_x >= static_cast<int>(width) ||
                curr_y < 0 || curr_y >= static_cast<int>(height)) {
                break; // Ray went out of bounds
            }

            // Check if we've gone too far (outside the radius)
            // Use squared distance for efficiency
            const int dist_squared = (curr_x - static_cast<int>(x)) * (curr_x - static_cast<int>(x)) +
                                     (curr_y - static_cast<int>(y)) * (curr_y - static_cast<int>(y));
            if (dist_squared > radius_squared) {
                break; // Ray went beyond the specified radius
            }

            // Get height at the current position on the ray
            const unsigned short point_height = height_map[curr_y * width + curr_x];

            // Calculate the vertical angle to this point
            // Use the precise distance for angle calculation
            const float distance = std::sqrt(static_cast<float>(dist_squared));
            const float height_diff = static_cast<float>(point_height) - static_cast<float>(current_height);
            const float angle = height_diff / distance;
            
            if (angle > max_angle_seen) {
                max_angle_seen = angle;
                visible_count++;
            }
        }
    }

    return visible_count;
}