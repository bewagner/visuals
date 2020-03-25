#! /usr/bin/env bash

base_directory=$(pwd)

opencv_build_directory="opencv_build"
rm -r "${opencv_build_directory}"
mkdir -p "${opencv_build_directory}"

cd ./opencv
rm -r build 
mkdir -p build 
cd ./build 
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local ..
make -j 8 -l 8



