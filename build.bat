@echo off
setlocal

cmake --preset debug
cmake --build --preset nj-debug -j 20
%~dp0build\debug\astar.exe
endlocal
