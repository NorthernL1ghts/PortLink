cmake_minimum_required(VERSION 3.20)

# Set the project name and language
project(PortLink VERSION 1.0 LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable from the src directory
add_executable(PortLink src/main.cpp)
