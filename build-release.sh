#!/bin/bash


cmake --preset release
cmake --build --preset nj-release -j
./build/debug/astar
