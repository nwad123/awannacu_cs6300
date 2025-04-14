// This file will use the MPI library to distribute the work of the Bresenham line algorithm across multiple processes.
// It will read in a hight map (16 bit raw) and find the output
// The output will be a 32bit file which will detect the number of visible pixels from every other cell.
// The maximum distance will be a radius of 100 pixels.
// each pixel in the output will be the number of visible pixels from that pixel

#include <cmath>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>

// #include <mpi.h> // Include MPI header for MPI functions
#include <algorithm> // For std::max
#include <iterator> // For std::istreambuf_iterator
#include <cstdlib> // For std::abs
#include <cstring> // For std::memcpy


// Bresenham's Line Algorithm: https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
struct Point
{
    int x;
    int y;
};


// Traces a line between points (x1, y1) and (x2, y2) and prints the intermediate coordinates
std::vector<Point> plot_line(int x1, int y1, int x2, int y2)
{
    // Create a vector to store the points
    std::vector<Point> points;
    points.reserve(std::max(abs(x2 - x1), abs(y2 - y1)) + 1); // Reserve space for the maximum number of points
	// Compute the differences between start and end points
	int dx = x2 - x1;
	int dy = y2 - y1;

	// Absolute values of the change in x and y
	const int abs_dx = abs(dx);
	const int abs_dy = abs(dy);

	// Initial point
	int x = x1;
	int y = y1;

	// Proceed based on the absolute differences to support all octants
	if (abs_dx > abs_dy)
	{
		// If the line is moving to the left, set dx accordingly
		int dx_update;
		if (dx > 0)
		{
			dx_update = 1;
		}
		else
		{
			dx_update = -1;
		}

		// Calculate the initial decision parameter
		int p = 2 * abs_dy - abs_dx;

		// Draw the line for the x-major case
		for (int i = 0; i <= abs_dx; i++)
		{
			// Print the current coordinate
			// std::cout << "(" << x << "," << y << ")" << std::endl;
            //add point to vector
            points.push_back({x, y});

			// Threshold for deciding whether or not to update y
			if (p < 0)
			{
				p = p + 2 * abs_dy;
			}
			else
			{
				// Update y
				if (dy >= 0)
				{
					y += 1;
				}
				else
				{
					y += -1;
				}

				p = p + 2 * abs_dy - 2 * abs_dx;
			}

			// Always update x
			x += dx_update;
		}
	}
	else
	{
		// If the line is moving downwards, set dy accordingly
		int dy_update;
		if (dy > 0)
		{
			dy_update = 1;
		}
		else
		{
			dy_update = -1;
		}

		// Calculate the initial decision parameter
		int p = 2 * abs_dx - abs_dy;

		// Draw the line for the y-major case
		for (int i = 0; i <= abs_dy; i++)
		{
			// Print the current coordinate
			// std::cout << "(" << x << "," << y << ")" << std::endl;
            points.push_back({x, y});
			// Threshold for deciding whether or not to update x
			if (p < 0)
			{
				p = p + 2 * abs_dx;
			}
			else
			{
				// Update x
				if (dx >= 0)
				{
					x += 1;
				}
				else
				{
					x += -1;
				}

				p = p + 2 * abs_dx - 2 * abs_dy;
			}

			// Always update y
			y += dy_update;
		}
	}

    //return value
    return points;
}
std::vector<Point> plot_line(Point start, Point end)
{
    return plot_line(start.x, start.y, end.x, end.y);
}


std::vector<unsigned int> calculateVisibility(const std::vector<unsigned short>& height_map, size_t width, size_t height, int radius = 100) {
    std::vector<unsigned int> visibility_map(width * height, 0);
    const int radius_squared = radius * radius;
    
    // Precompute the circle offsets once
    std::vector<std::pair<int, int>> circle_offsets;
    for (int dy = -radius; dy <= radius; ++dy) {
        for (int dx = -radius; dx <= radius; ++dx) {
            // Check if point is within circle
            if (dx*dx + dy*dy <= radius_squared) {
                circle_offsets.push_back({dx, dy});
            }
        }
    }
    
    // Process each pixel
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            // Print progress more frequently
            if ((y * width + x) % 1000 == 0) {
                std::cout << "\rProgress: " << (static_cast<float>(y * width + x) / (width * height)) * 100 << "%";
                std::cout.flush();
            }
            
            unsigned short current_height = height_map[y * width + x];
            // Skip walls
            if (current_height == 0) {
                visibility_map[y * width + x] = 0;
                continue;
            }
            
            // Start with 1 (the pixel itself is always visible)
            unsigned int visible_count = 1;
            
            // Check each point in the circle
            for (const auto& [dx, dy] : circle_offsets) {
                // Skip the center pixel (already counted)
                if (dx == 0 && dy == 0) continue;
                
                int target_x = static_cast<int>(x) + dx;
                int target_y = static_cast<int>(y) + dy;
                
                // Skip if out of bounds
                if (target_x < 0 || target_x >= static_cast<int>(width) || 
                    target_y < 0 || target_y >= static_cast<int>(height))
                    continue;
                
                // Skip walls
                unsigned short target_height = height_map[target_y * width + target_x];
                if (target_height == 0) continue;
                
                // Check line of sight using Bresenham's algorithm
                bool is_visible = true;
                
                // Calculate points along the line
                int x0 = x, y0 = y;
                int x1 = target_x, y1 = target_y;
                
                // Bresenham's line algorithm
                bool steep = std::abs(y1 - y0) > std::abs(x1 - x0);
                if (steep) {
                    std::swap(x0, y0);
                    std::swap(x1, y1);
                }
                
                if (x0 > x1) {
                    std::swap(x0, x1);
                    std::swap(y0, y1);
                }
                
                int dx_line = x1 - x0;
                int dy_line = std::abs(y1 - y0);
                int error = dx_line / 2;
                int ystep = (y0 < y1) ? 1 : -1;
                int y_line = y0;
                
                // Calculate slope for height check
                float dist_total = std::sqrt(static_cast<float>(dx_line * dx_line + dy_line * dy_line));
                float height_diff_total = target_height - current_height;
                float slope_threshold = height_diff_total / dist_total;
                
                // Check intermediate points along the line
                for (int x_line = x0 + 1; x_line < x1; ++x_line) {
                    int check_x = steep ? y_line : x_line;
                    int check_y = steep ? x_line : y_line;
                    
                    // Skip point check if it's one of the endpoints
                    if ((check_x == static_cast<int>(x) && check_y == static_cast<int>(y)) ||
                        (check_x == target_x && check_y == target_y))
                        continue;
                    
                    unsigned short check_height = height_map[check_y * width + check_x];
                    
                    // If we hit a wall, line of sight is blocked
                    if (check_height == 0) {
                        is_visible = false;
                        break;
                    }
                    
                    // Calculate the distance from source to this point
                    float dist_to_point = std::sqrt(static_cast<float>(
                        (check_x - static_cast<int>(x)) * (check_x - static_cast<int>(x)) + 
                        (check_y - static_cast<int>(y)) * (check_y - static_cast<int>(y))
                    ));
                    
                    // Calculate the minimum height needed to see over this point
                    float height_required = current_height + (slope_threshold * dist_to_point);
                    
                    // If this point's height exceeds what's required to see the target,
                    // then the target is blocked
                    if (check_height > height_required) {
                        is_visible = false;
                        break;
                    }
                    
                    // Update y position for Bresenham's algorithm
                    error -= dy_line;
                    if (error < 0) {
                        y_line += ystep;
                        error += dx_line;
                    }
                }
                
                // If the target is visible, increment the counter
                if (is_visible) {
                    visible_count++;
                }
            }
            
            // Store the visibility count
            visibility_map[y * width + x] = visible_count;
        }
    }
    
    std::cout << "\rProgress: 100%" << std::endl;
    return visibility_map;
}

int main(int argc, char** argv) {
    // Usage: ./<exec> <read_file> <write_file> <width> <height>
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <read_file> <write_file> <width> <height>" << std::endl;
        return 1;
    }
    
    // Parse width and height from command line
    int width = std::stoi(argv[3]);
    int height = std::stoi(argv[4]);
    size_t expected_size = width * height * sizeof(unsigned short);
    
    // Open input file
    std::ifstream input_file(argv[1], std::ios::binary);
    if (!input_file) {
        std::cerr << "Error opening input file: " << argv[1] << std::endl;
        return 1;
    }
    
    // Get file size
    input_file.seekg(0, std::ios::end);
    std::streamsize file_size = input_file.tellg();
    input_file.seekg(0, std::ios::beg);
    
    // Verify file size matches expected dimensions
    if (file_size != expected_size) {
        std::cerr << "Error: File size (" << file_size << " bytes) doesn't match expected dimensions: " 
                  << width << "x" << height << " (" << expected_size << " bytes)" << std::endl;
        return 1;
    }
    
    // Allocate memory for height map
    std::vector<unsigned short> height_map(width * height);
    
    // Read the file directly into the vector
    input_file.read(reinterpret_cast<char*>(height_map.data()), file_size);
    input_file.close();
    
    std::cout << "Height map loaded: " << width << "x" << height << std::endl;
    
    // Calculate visibility map
    int radius = 100;
    std::vector<unsigned int> visibility_map = calculateVisibility(height_map, width, height, radius);
    
    // Write output
    std::ofstream output_file(argv[2], std::ios::binary);
    if (!output_file) {
        std::cerr << "Error opening output file: " << argv[2] << std::endl;
        return 1;
    }
    
    output_file.write(reinterpret_cast<const char*>(visibility_map.data()), 
                      visibility_map.size() * sizeof(unsigned int));
    output_file.close();
    
    std::cout << "Output written to: " << argv[2] << std::endl;
    
    return 0;
}