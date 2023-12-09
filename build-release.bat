@echo off
setlocal

cmake --preset release
cmake --build --preset nj-release -j 20
%~dp0build\release\astar.exe
endlocal
