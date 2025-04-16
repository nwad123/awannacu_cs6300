# Disclaimer, entirely written by AI

import numpy as np
import cv2
import argparse

def display_raw_image(filename, width, height, window_name, dtype=np.int16):
    """
    Reads a raw image file, converts it to a displayable format, and shows it in a window.

    Args:
        filename (str): The path to the .raw image file.
        width (int): The width of the image. 
        height (int): The height of the image.
        dtype (numpy.dtype, optional): The data type of the raw image file. Defaults to np.int16.
    """
    try:
        # Read the raw data from the file
        with open(filename, 'rb') as f:
            raw_data = f.read()

        # Convert the raw data to a NumPy array with the specified data type
        image_array = np.frombuffer(raw_data, dtype=dtype)

        # Reshape the 1D array into a 2D array representing the image
        image_array = image_array.reshape((height, width))

        # --- Important Image Processing for Display ---
        # 1. Clip or Normalize: Handle the int16 data appropriately.
        #    - If your height data has a known range (e.g., 0-10000), clip it.
        #    - Otherwise, normalize it to the range 0-255 for display.        
        min_val = np.min(image_array)
        max_val = np.max(image_array)
        normalized_image = ((image_array - min_val) / (max_val - min_val) * 255).astype(np.uint8)
        # 2. Color Mapping (Optional): Apply a colormap for better visualization.
        #    - This makes height variations more apparent.  Try different colormaps.
        #    -  cv2.COLORMAP_JET, cv2.COLORMAP_TERRAIN, cv2.COLORMAP_HOT are good choices.
        # color_mapped_image = cv2.applyColorMap(normalized_image, cv2.COLORMAP_BONE)

        print(f"Range: [{min_val}:{max_val}]")

        # Display the image in a window
        cv2.imshow(window_name, normalized_image)
    except FileNotFoundError:
        print(f"Error: File not found at {filename}")
    except Exception as e:
        print(f"An error occurred: {e}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Display a raw image file.")
    parser.add_argument("width", type=int, help="Width of the image")
    parser.add_argument("height", type=int, help="Height of the image")
    parser.add_argument("filenames", nargs='+', help="Paths to the raw image files")
    args = parser.parse_args()


    width = args.width
    height = args.height
    filenames = args.filenames

    for i, filename in enumerate(filenames):
        window_name = f"Raw Image {i+1}"
        display_raw_image(filename, width, height, window_name)
    
    cv2.waitKey(0)  # Wait for a key press to close the window
    cv2.destroyAllWindows()  # Clean up
