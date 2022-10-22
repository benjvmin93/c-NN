from PIL import Image

def resize_image(path):
    image = Image.open(path)
    new_image = image.resize((300, 300))
    new_image.save('normalized-images/normalized-' + path)