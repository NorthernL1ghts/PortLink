@echo off

:: Go up one directory from the location of the script
cd ..

:: Check if the 'build' directory exists; if not, create it
if not exist build (
    mkdir build
)

:: Change to the 'build' directory where the CMake configuration will take place
cd build

:: Run CMake to generate build files for Visual Studio 2022 (default generator)
cmake .. -G "Visual Studio 17 2022"

:: Build the project using the generated Visual Studio solution files
cmake --build . --config Release

:: Pause to keep the terminal open, allowing you to see any output
pause
