#!/bin/bash


cmake --preset debug
cmake --build --preset nj-debug
./build/debug/astar
