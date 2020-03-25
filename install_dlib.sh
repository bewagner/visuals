#! /usr/bin/env bash

cd dlib
mkdir -p build
cd build
cmake ..
make -j 8 -l 8
sudo make install
