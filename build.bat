@echo off
setlocal

REM
set BUILD_DIR=build


REM Check for --clean
if "%1"=="--clean" (
    echo Cleaning build directory...
    if exist %BUILD_DIR% (
        rmdir /s /q "%BUILD_DIR%"
    )
)

if not exist "%BUILD_DIR%" (
    mkdir "%BUILD_DIR%"
)

cd "%BUILD_DIR%"

cmake ..
cmake --build .

endlocal
