#Here The Main things were how i do for the cropping Part

#Reference-https://pixspy.com/
#I used this website to Know each pixel coordinate.

#Rules which i Used while Calculating The actual Distance
# 1)I Primarily Checked the righmost,leftmost,topmost,and bottom Pixel of the Bubble 
# 2)It was as Following
#      ->(396,y) if considered the leftmost Part(white One) + 1 (For Black One (255,255,255))
#      ->(x,597) if considered the topmost Part(white One)  + 1 (for Black One (255,255,255))
#      ->(504,y) if considered the rightmost Part(white One)  - 1 (for Black One (255,255,255))
#      ->(x,707) if considered the bottom Part(white One)  - 1 (for Black One (255,255,255))

from PIL import Image

# Load the image
image = Image.open("Sample-1-B&W.png")

# Define crop coordinates (left, top, right, bottom)
x1, y1 = 397, 598  # Top-left corner of the bubble
x2, y2 = 503, 706  # Bottom-right corner of the bubble

# Ensure coordinates are within image bounds
width, height = image.size
x2 = min(x2, width)
y2 = min(y2, height)

# Crop the bubble
cropped_bubble = image.crop((x1, y1, x2, y2))

# Save the result
cropped_bubble.save("cropped_bubble.png")
print("Bubble cropped successfully!")
