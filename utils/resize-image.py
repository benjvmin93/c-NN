from PIL import Image

import sys

def usage():
    print('USAGE: python3 resize-image.py ${IMAGE-TO-RESIZE}')
    exit(1)

def resize_image(path):
    image = Image.open(path)
    new_image = image.resize((300, 300))
    new_image.save('normalized-images/normalized-' + path)

if __name__ == '__main__':
    args = sys.argv
    if len(args) == 1:
        usage()
    resize_image(args[1])
