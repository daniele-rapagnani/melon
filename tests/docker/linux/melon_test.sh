#!/bin/bash

cd /opt/melon
mkdir -p build-linux
cd build-linux
cmake --debug-output -DCMAKE_BUILD_TYPE=Debug -DSTRESS_GC=ON ../
make -j4
cd ../tests
../build-linux/tests