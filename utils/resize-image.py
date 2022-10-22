from PIL import Image
import sys

def usage():
    print('USAGE: python3 resize-image.py ${IMAGE-TO-RESIZE}')
    exit(1)

def resize_image(path):
    split = path.split('/')
    name = split[len(split) - 1]
    new_path = 'utils/normalized-images/normalized-' + name

    image = Image.open(path)
    if image.size == (300, 300):
        image.save(new_path)
        return 0
    new_image = image.resize((300, 300))
    new_image.save(new_path)
    return 0

if __name__ == '__main__':
    args = sys.argv
    if len(args) == 1:
        usage()
    resize_image(args[1])
