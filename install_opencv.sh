#! /usr/bin/env bash

base_directory=$(pwd)

opencv_build_directory="opencv_build"

cd ./opencv
mkdir -p build 
cd ./build 
cmake -DCMAKE_BUILD_TYPE=Release -DOPENCV_EXTRA_MODULES_PATH=${base_directory}/opencv_contrib/modules -DCMAKE_INSTALL_PREFIX=/usr/local -DOPENCV_GENERATE_PKGCONFIG=ON ..
make -j $(nproc) -l $(nproc)
sudo make install
