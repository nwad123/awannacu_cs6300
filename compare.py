import numpy as np
import cv2
import argparse

def compare_raw_images(filename1, filename2, width, height, dtype=np.int16):
    """
    Compares two raw image files, displays their differences, and saves the difference image.

    Args:
        filename1 (str): The path to the first .raw image file.
        filename2 (str): The path to the second .raw image file.
        width (int): The width of the images.
        height (int): The height of the images.
        dtype (numpy.dtype, optional): The data type of the raw image files. Defaults to np.int16.
    """
    try:
        # Read the raw data from the first file
        with open(filename1, 'rb') as f:
            raw_data1 = f.read()
        image_array1 = np.frombuffer(raw_data1, dtype=dtype).reshape((height, width))

        # Read the raw data from the second file
        with open(filename2, 'rb') as f:
            raw_data2 = f.read()
        image_array2 = np.frombuffer(raw_data2, dtype=dtype).reshape((height, width))

        # Calculate the absolute difference between the two images
        diff_array = np.abs(image_array1 - image_array2)

        # Normalize the difference array for display
        min_val = np.min(diff_array)
        max_val = np.max(diff_array)
        if max_val > 0:
            normalized_diff = ((diff_array - min_val) / (max_val - min_val) * 255).astype(np.uint8)
        else:
            normalized_diff = np.zeros_like(diff_array, dtype=np.uint8)

        if max_val == 0:
            print("The images are identical with no differences.")

        if max_val <= 2.0:
            print("The images are similar.")

        # Apply a colormap to the difference image
        color_mapped_diff = cv2.applyColorMap(normalized_diff, cv2.COLORMAP_JET)

        # Display the difference image
        cv2.imshow("Difference Image", color_mapped_diff)
        cv2.waitKey(0)
        cv2.destroyAllWindows()

    except FileNotFoundError:
        print(f"Error: One or both files not found.")
    except Exception as e:
        print(f"An error occurred: {e}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Compare two raw image files.")
    parser.add_argument("filename1", help="Path to the first raw image file")
    parser.add_argument("filename2", help="Path to the second raw image file")
    parser.add_argument("width", type=int, help="Width of the images")
    parser.add_argument("height", type=int, help="Height of the images")
    args = parser.parse_args()

    filename1 = args.filename1
    filename2 = args.filename2
    width = args.width
    height = args.height

    compare_raw_images(filename1, filename2, width, height)
