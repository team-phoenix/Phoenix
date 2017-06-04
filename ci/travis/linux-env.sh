#!/bin/bash (This file should be sourced rather than executed.)
# Sourcing allows environment variables to be used outside the script, but other
# changes also persist. (If you change directory then remember to change back!)

if [ "$(basename "$0")" == "environment.sh" ]; then
  echo "$0 was executed but it should have been sourced."
  exit 1
fi

initial_dir="${PWD}"
set -e # exit on error
set -x # echo commands

# Set up compilers
export CC=/usr/bin/gcc-4.9
export CXX=/usr/bin/g++-4.9

# Set up CMake
CMAKE_VERSION_MAJOR=3
CMAKE_VERSION_MINOR=8
CMAKE_VERSION_PATCH=2
CMAKE_VERSION="${CMAKE_VERSION_MAJOR}.${CMAKE_VERSION_MINOR}.${CMAKE_VERSION_PATCH}"
CMAKE_VERSION_MAJOR_DOT_MINOR="${CMAKE_VERSION_MAJOR}.${CMAKE_VERSION_MINOR}"
CMAKE_URL="http://www.cmake.org/files/v${CMAKE_VERSION_MAJOR_DOT_MINOR}/cmake-${CMAKE_VERSION}-Linux-x86_64.tar.gz"
mkdir cmake && travis_retry wget --no-check-certificate --quiet -O - ${CMAKE_URL} | tar --strip-components=1 -xz -C cmake
export PATH=${PWD}/cmake/bin:${PATH}

# Set up Qt
QT_VERSION_MAJOR=5
QT_VERSION_MINOR=9
QT_VERSION_PATCH=0
QT_VERSION_MAJORMINOR="${QT_VERSION_MAJOR}${QT_VERSION_MINOR}"
QT_VERSION_MAJORMINORPATCH="${QT_VERSION_MAJOR}${QT_VERSION_MINOR}${QT_VERSION_PATCH}"
QT_VERSION="${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}"
mkdir "qt5" && travis_retry wget -O "qt5.tar.gz" "http://s3.amazonaws.com/vg.phoenix.ci.utils/qt-${QT_VERSION}-linux-gcc_64.tar.gz"
tar -xf "qt5.tar.gz" -C "qt5"
export PATH="${PWD}/qt5/bin:$PATH"
export QT_PLUGIN_PATH="${PWD}/qt5/plugins"
export QML2_IMPORT_PATH="${PWD}/qt5/qml"

# Artifact uploading
wget "https://s3.amazonaws.com/travis-ci-gmbh/artifacts/stable/build/linux/amd64/artifacts"
chmod +x artifacts
export PATH=`pwd`:$PATH

# Return shell to its initial state:
set +x
set +e
cd "${initial_dir}"