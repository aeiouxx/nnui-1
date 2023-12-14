@echo off
setlocal

if not exist build mkdir build
cd build
cmake .. -DASTAR_LOGGER_ENABLE=ON 
cmake --build . --target ALL_BUILD --config Debug -- /nologo /verbosity:minimal /maxcpucount
%~dp0build\debug\astar.exe
endlocal
