#!/bin/bash

# Go up one directory from the location of the script
cd ..

# Check if the 'build' directory exists; if not, create it
if [ ! -d "build" ]; then
    mkdir build
fi

# Change to the 'build' directory where the CMake configuration will take place
cd build

# Set the build type (can be PL_DEBUG, PL_RELEASE, or PL_DIST)
BUILD_TYPE="PL_DEBUG"

# Run CMake to generate build files
cmake .. -DCMAKE_BUILD_TYPE=$BUILD_TYPE

# Build the project using the generated makefiles
cmake --build . --config $BUILD_TYPE

# Pause to keep the terminal open, allowing you to see any output (only for Linux/MacOS)
echo "Press any key to continue..."
read -n 1 -s
