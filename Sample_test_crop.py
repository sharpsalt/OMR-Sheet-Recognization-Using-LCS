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
image1 = Image.open("Sample-1-B&W.png")
image2 = Image.open("Sample-2-B&W.png")

# Define crop coordinates (left, top, right, bottom)
x1, y1 = 397, 598  # Top-left corner of the bubble
x2, y2 = 503, 706  # Bottom-right corner of the bubble

# Ensure coordinates are within image bounds
width1, height1 = image1.size
width2, height2 = image2.size
x2 = min(x2, width1)
y2 = min(y2, height1)

# Crop the bubble
cropped_bubble1 = image1.crop((x1, y1, x2, y2))
cropped_bubble2 = image2.crop((x1, y1, x2, y2))

# Save the result
cropped_bubble1.save("cropped_bubble1.png")
cropped_bubble2.save("cropped_bubble2.png")
print("Bubble cropped successfully!")
