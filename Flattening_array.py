from PIL import Image
import numpy as np

def flatten_and_save_png(image_path, output_txt_file):
    """
    Flatten a PNG image into a 1D array and save it to a text file.
    Args:
        image_path (str): Path to the input PNG image.
        output_txt_file (str): Path to save the flattened array as a text file.
    """
    try:
        # Open and convert the image to RGB (handles transparency)
        img = Image.open(image_path).convert('RGB')
        
        # Convert to numpy array and flatten into 1D
        flattened_array = np.array(img).flatten()
        
        # Save the array to a text file (space-separated values)
        np.savetxt(output_txt_file, flattened_array, fmt='%d')
        
        print(f" Flattened array saved to: {output_txt_file}")
    except Exception as e:
        print(f" Error processing {image_path}: {e}")

if __name__ == "__main__":
    input_pngs = ['cropped_bubble1.png', 'cropped_bubble2.png']
    output_txts = ['Test.txt', 'Sample.txt']
    for i in range(len(input_pngs)):
        flatten_and_save_png(input_pngs[i], output_txts[i])