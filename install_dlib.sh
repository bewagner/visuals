#! /usr/bin/env bash

cd dlib
mkdir -p build
cd build
cmake -DUSE_AVX_INSTRUCTIONS=ON -DUSE_SSE2_INSTRUCTIONS=ON -DUSE_SSE4_INSTRUCTIONS=ON ..
make -j 8 -l 8
sudo make install
