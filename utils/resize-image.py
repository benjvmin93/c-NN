from PIL import Image
import sys
import os

def usage():
    print('USAGE: python3 resize-image.py ${IMAGE-FOLDER-TO-RESIZE}')
    exit(1)

def resize_image(path):
    print('Starting normalization of images inside directory', path, '\n')
    for dir in os.listdir(path):
        print('Entering directory', dir)
        new_path = 'utils/normalized-images/' + dir
        if not os.path.exists(new_path):
            os.mkdir(new_path)
        for filename in os.listdir(path + dir):
            imagePath = os.path.join(path, dir)
            imagePath = os.path.join(imagePath, filename)
            image = Image.open(imagePath)
            if image.size != (300, 300):
                new_image = image.resize((300, 300))
            new_image.save(new_path + '/' + filename + '.jpg')
            print('Saved ' + new_path + '/' + filename + '.jpg')
        print()
    print('Normalization ended.')
    return 0

if __name__ == '__main__':
    args = sys.argv
    if len(args) != 2:
        usage()
    resize_image(args[1])
