# Disclaimer, entirely written by AI

import numpy as np
import cv2
import argparse

def display_raw_image(filename, width, height, window_name, dtype):
    """
    Reads a raw image file, converts it to a displayable format, and shows it in a window.

    Args:
        filename (str): The path to the .raw image file.
        width (int): The width of the image. 
        height (int): The height of the image.
        dtype (numpy.dtype): The data type of the raw image file.
    """
    try:
        # Read the raw data from the file
        with open(filename, 'rb') as f:
            raw_data = f.read()

        # Convert the raw data to a NumPy array with the specified data type
        image_array = np.frombuffer(raw_data, dtype=dtype)

        # Reshape the 1D array into a 2D array representing the image
        image_array = image_array.reshape((height, width))

        # Normalize the image array to the range 0-255 for display
        max_val = np.max(image_array)
        min_val = np.min(image_array)

        image_array = image_array - min_val
        max_2 = np.max(image_array);
        if max_2 > 0:
            image_array = (image_array / max_2 * 255).astype(np.uint8)
        else:
            image_array = np.zeros_like(image_array, dtype=np.uint8)

        print(f"Filename: {filename}, Min: {min_val}, Max: {max_val}")

        # Display the image in a window
        cv2.imshow(window_name, image_array)        
    except FileNotFoundError:
        print(f"Error: File not found at {filename}")
    except Exception as e:
        print(f"An error occurred: {e}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Display a raw image file.")
    parser.add_argument("width", type=int, help="Width of the image")
    parser.add_argument("height", type=int, help="Height of the image")
    parser.add_argument("dtype", choices=["np.int16", "np.int32"], help="Data type of the raw image file (np.int16 or np.int32)")
    parser.add_argument("filenames", nargs='+', help="Paths to the raw image files")
    
    args = parser.parse_args()

    width = args.width
    height = args.height
    
    if args.dtype == "np.int16":
        dtype = np.int16
    elif args.dtype == "np.int32":
        dtype = np.int32
    else:
        print("Invalid data type specified.")
        exit()

    filenames = args.filenames

    for i, filename in enumerate(filenames):
        window_name = f"Raw Image {i+1}: {filename}"
        display_raw_image(filename, width, height, window_name, dtype)
    
    cv2.waitKey(0)  # Wait for a key press to close the window
    cv2.destroyAllWindows()  # Clean up
