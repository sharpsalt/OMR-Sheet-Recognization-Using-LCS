from PIL import Image
import numpy as np

def convert_to_bw_with_threshold(image_path, threshold=128):
    #We will Load the Image
    image = Image.open(image_path).convert("L")  # First convert to grayscale
    img_array = np.array(image)
    
    # Apply threshold (pixels > threshold become white, else black)
    bw_array = np.where(img_array > threshold, 255, 0)
    
    # Convert back to PIL image
    bw_image = Image.fromarray(bw_array.astype('uint8'))
    
    # Save and show
    if(image_path=="Sample-1.png"):
        bw_image.save("Sample-1-B&W.png")
    elif(image_path=="Sample-2.png"):
        bw_image.save("Sample-2-B&W.png")
    bw_image.show()

convert_to_bw_with_threshold("Sample-1.png", threshold=150)
convert_to_bw_with_threshold("Sample-2.png", threshold=150)

#Basically , to use it Primarily you have to install Pillow Numpy
#pip install pillow numpy (which you can use to get it install after that you can use it )...