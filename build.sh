#!/bin/bash


cmake --preset debug
cmake --build --preset nj-debug -j 20
./build/debug/astar
