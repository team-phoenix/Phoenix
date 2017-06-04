#!/bin/bash

mkdir "build" "install"
cmake .. -DCMAKE_CXX_COMPILER_LAUNCHER=ccache -DCMAKE_INSTALL_PREFIX="${PWD}/install"
cmake --build ./build --target "build"
cmake --build ./build --target "install"
