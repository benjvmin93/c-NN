#!/bin/sh

# Install requirements for arch distro
if ! [ -x "$(command -v pip)" ]; then
    sudo pacman -S python-pip
fi

python3 -m pip install --upgrade pip
python3 -m pip install --upgrade Pillow