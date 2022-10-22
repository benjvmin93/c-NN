# Basic CNN

This project is aiming to learn more about neural networks and especially convolutional neural networks. It aims to recognize images such as humans, animals, vehicules and objects.

## Informations

Each data passed to the neural network has to be normalized to maintain consistency over the learning of such a neural network. The choosen size is 300x300 pixels.

At the moment, the forward propagation is working just fine.I am still trying to implement a correct back propagation though.

## Install dependencies

### This project requires SDL2 library. To get it, just download and install the library according to your distro preference.

https://github.com/libsdl-org/SDL/releases/tag/release-2.24.1

### Then you need to install Pillow for image normalization. To do so, juste run 
```./install-dep.sh```

## Normalize your images

```python3 resize-image.py ${IMAGE_TO_NORMALIZE}```

## Get started

### Training

Not implemented yet.

### Run

To try the network on one of your image, just run

```make```

```./CNN [--verbose] ${PATH-TO-IMAGE}```

Verbose option allows to see every step of the prediction process including convolution and pooling outputs.
Don't forget to normalize your image.