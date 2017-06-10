#!/bin/bash

set -e # exit on error
set -x # echo commands

cd ..
mkdir "build" "install"
cd build
cmake -DCMAKE_CXX_COMPILER_LAUNCHER="ccache" -DCMAKE_INSTALL_PREFIX="${PWD}/../install" ./Phoenix
cmake --build .
cmake --build . --target "install"
