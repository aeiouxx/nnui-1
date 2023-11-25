#!/usr/bin/bash
clear_build_directory() {
    echo "Cleaning build directory..."
    rm -rf build
}

if [ "$1" == "--clean" ]; then
	clear_build_directory
fi

mkdir -p out 
cd build
cmake ..
make
