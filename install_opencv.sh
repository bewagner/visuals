#! /usr/bin/env bash

set -e

echo "--- Update git submodules ---"
git submodule update --init --recursive

echo "--- Install openCV dependencies ---"
sudo apt-get -y install build-essential
sudo apt-get -y install cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev
sudo apt-get -y install python-dev python-numpy libtbb2 libtbb-dev libjpeg-dev libpng-dev libtiff-dev libdc1394-22-dev

echo "--- Build openCV ---"
base_directory=$(pwd)
opencv_build_directory="opencv_build"
cd ./opencv
mkdir -p build 
cd ./build 
cmake -DCMAKE_BUILD_TYPE=Release \
      -DENABLE_FAST_MATH=ON \
      -DINSTALL_C_EXAMPLES=OFF \
      -DINSTALL_PYTHON_EXAMPLES=OFF \
      -DBUILD_TESTS=OFF \
      -DBUILD_PERF_TESTS=OFF \
      -DBUILD_EXAMPLES=OFF \
      -DBUILD_DOCS=OFF \
      -DOPENCV_EXTRA_MODULES_PATH=${base_directory}/opencv_contrib/modules \
      -DCMAKE_INSTALL_PREFIX=/usr/local ..
make -j $(nproc) -l $(nproc)

echo "--- Install openCV ---"
sudo make install
