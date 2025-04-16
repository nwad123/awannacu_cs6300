def bresenham_line(x0, y0, x1, y1):
    """
    Generates the points of a 2D line using Bresenham's algorithm.

    Args:
        x0: The x-coordinate of the starting point.
        y0: The y-coordinate of the starting point.
        x1: The x-coordinate of the ending point.
        y1: The y-coordinate of the ending point.

    Returns:
        A list of (x, y) tuples representing the points on the line.
    """
    points = []
    dx = abs(x1 - x0)
    dy = abs(y1 - y0)
    sx = 1 if x0 < x1 else -1
    sy = 1 if y0 < y1 else -1
    err = dx - dy

    x = x0
    y = y0
    while True:
        points.append((x, y))
        if x == x1 and y == y1:
            break
        e2 = 2 * err
        if e2 > -dy:
            err -= dy
            x += sx
        if e2 < dx:
            err += dx
            y += sy
    return points

def display_line_points(points, x0, y0, x1, y1):
    """
    Displays the line points on a grid using asterisks.

    Args:
        points: A list of (x, y) tuples representing the points on the line.
        x0: The x-coordinate of the starting point.
        y0: The y-coordinate of the starting point.
        x1: The x-coordinate of the ending point.
        y1: The y-coordinate of the ending point.
    """
    # Determine the boundaries of the grid.
    min_x = min(x0, x1, min(p[0] for p in points))
    max_x = max(x0, x1, max(p[0] for p in points))
    min_y = min(y0, y1, min(p[1] for p in points))
    max_y = max(y0, y1, max(p[1] for p in points))

    # Calculate the size of the grid.
    width = max_x - min_x + 1
    height = max_y - min_y + 1

    # Create an empty grid.
    grid = [[' ' for _ in range(width)] for _ in range(height)]

    # Mark the points on the grid.
    for x, y in points:
        # Shift the coordinates to fit within the grid.
        grid_x = x - min_x
        grid_y = height - 1 - (y - min_y)  # Invert y-axis for display
        if 0 <= grid_x < width and 0 <= grid_y < height:
            grid[grid_y][grid_x] = '*'

    # Print the grid.
    print(f"Bresenham Line from ({x0}, {y0}) to ({x1}, {y1}):")
    for row in grid:
        print(''.join(row))

def main():
    """
    Example usage of the bresenham_line function and displays the
    output in a visual manner.
    """
    # Define the start and end points of the line.
    x0, y0 = 0, 0
    x1, y1 = 5, 2

    # Generate the points on the line.
    line_points = bresenham_line(x0, y0, x1, y1)

    # Display the points.
    display_line_points(line_points, x0, y0, x1, y1)

    # Example with different points
    x0, y0 = 1, 3
    x1, y1 = 8, 10
    line_points2 = bresenham_line(x0, y0, x1, y1)
    display_line_points(line_points2, x0, y0, x1, y1)


if __name__ == "__main__":
    main()
