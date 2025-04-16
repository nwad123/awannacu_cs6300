import math

def get_circle_points_simplified(radius):
    """
    Generates the minimum number of points to cover a circle on a pixel grid,
    assuming the center is at (0, 0).

    Args:
        radius: The radius of the circle (an integer).

    Returns:
        A list of (x, y) tuples representing the points on the circle's edge.
    """
    if radius <= 0:
        raise ValueError("Radius must be positive.")

    # Calculate the estimated circumference.
    circumference = 2.0 * math.pi * radius
    # Estimate the number of points.
    estimated_num_points = int(math.ceil(circumference))

    # Initialize the list of points.
    points = []
    x = 0
    y = radius
    d = 3 - 2 * radius  # Initial decision parameter

    while x <= y:
        points.append((x, y))
        points.append((y, x))
        points.append((-x, y))
        points.append((-y, x))
        points.append((x, -y))
        points.append((y, -x))
        points.append((-x, -y))
        points.append((-y, -x))
        if d < 0:
            d = d + 4 * x + 6
        else:
            d = d + 4 * (x - y) + 10
            y -= 1
        x += 1

    print(f"Estimated Number of Points: {estimated_num_points}, Actual Number of Points: {len(points)}")
    return points

def display_circle_points(points, radius):
    """
    Displays the circle points on a grid using asterisks.

    Args:
        points: A list of (x, y) tuples representing the points on the circle's edge.
        radius: The radius of the circle.
    """
    # Determine the size of the grid.
    size = 2 * radius + 1
    # Create an empty grid.
    grid = [['  ' for _ in range(2*size)] for _ in range(size)]

    # Mark the points on the grid.
    for x, y in points:
        # Shift the coordinates to the center of the grid.
        grid_x = x + radius
        grid_y = radius - y  # Invert y-axis for display
        if 0 <= grid_x < size and 0 <= grid_y < size:
            grid[grid_y][grid_x] = '*'
            grid[grid_y][grid_x + 1] = '*'

    # Print the grid.
    for row in grid:
        print(''.join(row))

def main():
    """
    Example usage of the get_circle_points_simplified function and displays the
    output in a visual manner.
    """
    # Get the points for a circle with radius 10.  Adjust radius for better visual output.
    radius = 20
    circle_points = get_circle_points_simplified(radius)

    # Display the points.
    print(f"Circle Points (Radius {radius}):")
    display_circle_points(circle_points, radius)

if __name__ == "__main__":
    main()
