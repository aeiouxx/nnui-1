@echo off
setlocal

if not exist build mkdir build
cd build
cmake ..
cmake --build . --target ALL_BUILD --config Release -- /nologo /verbosity:minimal /maxcpucount
%~dp0build\release\astar.exe
endlocal
