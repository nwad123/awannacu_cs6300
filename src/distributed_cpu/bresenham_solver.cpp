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

std::vector<unsigned int> calculateVisibility(const std::vector<unsigned short>& height_map, 
    size_t width, size_t height, int radius = 100) {
std::vector<unsigned int> visibility_map(width * height, 0);
const int radius_squared = radius * radius;

// Optimization 1: Precompute angles in each direction for faster line-of-sight checks
const int num_angles = 360;  // Number of discrete angles
const double angle_step = 2 * M_PI / num_angles;

std::vector<std::pair<int, int>> ray_directions;
ray_directions.reserve(num_angles);

for (int i = 0; i < num_angles; ++i) {
double angle = i * angle_step;
int dx = static_cast<int>(std::round(std::cos(angle) * radius));
int dy = static_cast<int>(std::round(std::sin(angle) * radius));
ray_directions.push_back({dx, dy});
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
if (current_height == 0) continue;

// Start with 1 (the pixel itself is always visible)
unsigned int visible_count = 1;

// Cast rays in different directions
for (const auto& [dx, dy] : ray_directions) {
// Use a more efficient ray casting approach
// Start from the center and move outward
int max_steps = radius;
int curr_x = x;
int curr_y = y;
float max_angle_seen = -std::numeric_limits<float>::infinity();

// Step size for more efficient ray traversal
// For long rays, we don't need to check every pixel
const float ray_length = std::sqrt(dx*dx + dy*dy);
const float step_x = dx / ray_length;
const float step_y = dy / ray_length;

float curr_x_f = x + 0.5f;  // Start at center of pixel
float curr_y_f = y + 0.5f;

bool hit_wall = false;

for (int step = 1; step <= max_steps; ++step) {
// Move along the ray
curr_x_f += step_x;
curr_y_f += step_y;

// Round to nearest pixel
curr_x = std::round(curr_x_f);
curr_y = std::round(curr_y_f);

// Check bounds
if (curr_x < 0 || curr_x >= static_cast<int>(width) || 
curr_y < 0 || curr_y >= static_cast<int>(height))
break;

// Check if we've gone too far (outside the radius)
int dist_squared = (curr_x - x)*(curr_x - x) + (curr_y - y)*(curr_y - y);
if (dist_squared > radius_squared)
break;

// Get height at current position
unsigned short point_height = height_map[curr_y * width + curr_x];

// Skip if it's a wall (height 0)
if (point_height == 0) {
hit_wall = true;
break;
}

// Calculate angle to determine visibility
float distance = std::sqrt(dist_squared);
float height_diff = point_height - current_height;
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
visibility_map[y * width + x] = visible_count;
}
}

std::cout << "\rProgress: 100%" << std::endl;
return visibility_map;
}

// Example of how to use plot_line, I show that the algorithm will work from any starting point to any stopping point, regardless of direction
int main(int argc, char** argv)
{
	//usage: ./<exec> <read_file> <write_file> <mpi_rank> <mpi_size>

    // Check if the correct number of arguments is provided
    if (argc != 5){
        std::cerr << "Usage: " << argv[0] << " <read_file> <write_file> <mpi_rank> <mpi_size>" << std::endl;
        return 1;
    }
    // Read the input file from the command line arguments into memory
    std::ifstream input_file(argv[1], std::ios::binary);
    if (!input_file){
        std::cerr << "Error opening input file: " << argv[1] << std::endl;
        return 1;
    }
    // Read the file into a vector of unsigned shorts (16-bit values)
    std::vector<unsigned short> height_map((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
    input_file.close();
    // Check if the file was read successfully
    if (height_map.empty()){
        std::cerr << "Error reading height map from file: " << argv[1] << std::endl;
        return 1;
    }
    // Get the size of the height map
    size_t height_map_size = height_map.size() / sizeof(unsigned short);
    // Calculate the width and height of the height map assume square
    size_t width = static_cast<size_t>(sqrt(height_map_size));
    size_t height = height_map_size / width;
    // Check if the height map is square
    if (width * height != height_map_size){
        std::cerr << "Error: Height map is not square." << std::endl;
        return 1;
    }

    // Get the rank and size of the MPI processes
    int rank = std::stoi(argv[3]);
    int size = std::stoi(argv[4]);
    // Check if the rank and size are valid
    // if (rank < 0 || rank >= size){
    //     std::cerr << "Error: Invalid MPI rank or size." << std::endl;
    //     return 1;
    // }
    // Check if the rank is 0, if so, print the height map size
    // if (rank == 0){
        std::cout << "Height map size: " << height_map_size << std::endl;
        std::cout << "Width: " << width << std::endl;
        std::cout << "Height: " << height << std::endl;
    // }

    // Create a vector to store the output image
    std::vector<unsigned int> output_image(height_map_size, 0); // Initialize to 0
    

    // Calculate the visibility map using the height map and the radius
    int radius = 100; // Set the radius for visibility calculation
    std::vector<unsigned int> visibility_map = calculateVisibility(height_map, width, height, radius);
    
    // Write the output image to a file
    std::ofstream output_file(argv[2], std::ios::binary);
    if (!output_file){
        std::cerr << "Error opening output file: " << argv[2] << std::endl;
        return 1;
    }
    // Write the output image to the file, give it a 32bit raw header p5
    // output_file << "P5\n" << width << " " << height << "\n255\n"; // P5 header for binary PGM
    output_file.write(reinterpret_cast<const char*>(visibility_map.data()), visibility_map.size() * sizeof(unsigned int));
    output_file.close();
    // Check if the output file was written successfully
    if (!output_file){
        std::cerr << "Error writing output file: " << argv[2] << std::endl;
        return 1;
    }
    // Print the output image size

    if (rank == 0){
        std::cout << "Output image size: " << output_image.size() << std::endl;
        std::cout << "Output image written to: " << argv[2] << std::endl;
    }

	
	return 0;
}