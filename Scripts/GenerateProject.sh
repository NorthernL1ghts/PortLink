#!/bin/bash

# Move up one directory from the script's location
cd ..

# Create the build directory if it doesn't exist
if [ ! -d "build" ]; then
  mkdir build
fi

# Navigate into the build directory
cd build

# Run CMake with the Visual Studio 2022 generator
cmake .. -G "Visual Studio 17 2022"

# Build the project in Release configuration
cmake --build . --config Release
