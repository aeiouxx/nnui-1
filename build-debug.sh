#!/bin/bash


cmake --preset debug
cmake --build --preset nj-debug -j
./build/debug/astar
