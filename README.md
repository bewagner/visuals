# Building a face detector with OpenCV in C++

[![Build Status](https://travis-ci.com/bewagner/visuals.svg?branch=blog-post-1)](https://travis-ci.com/bewagner/visuals)

Here you find the code for my [blog post](http://bewagner.github.io/programming/2020/04/12/building-a-face-detector-with-opencv-in-cpp/) on building a face detector with OpenCV. 

<p align="center">
  <img width="460" height="300" src="/images/detectingFaces.gif">
</p>

## Installing OpenCV

Run 
```shell script 
./install_opencv.sh
```
to install `opencv` and the `opencv_contrib` module. This script will
- pull in the `opencv` and `opencv_contrib` submodules
- install the necessary system dependencies
- build and install `opencv` and `opencv_contrib`
 
## Building

Build this directory in your favorite C++ IDE or run
```
mkdir build && cd build
cmake ..
cmake --build .
```
to build the code. Let me know if you run into any problems!

## Which versions I used
This code has been tested with
- Ubuntu 18.04
- OpenCV 4.2.0 

If you run `./install_opencv.sh` or `git submodule update --init --recursive`, the local submodule will be set to the correct version of OpenCV.

