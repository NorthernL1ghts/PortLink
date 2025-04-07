# Specify the minimum version of CMake required
cmake_minimum_required(VERSION 3.20)

# Set the project name and language
project(PortLink VERSION 1.0 LANGUAGES CXX)

# Only support x64 architecture
set(CMAKE_GENERATOR_PLATFORM x64)

# Set the C++ standard and enforce it
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Specify additional directories for the project
include_directories(src)

# Add the executable from the src directory
add_executable(PortLink src/main.cpp)

# Set the output and intermediate directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries/${CMAKE_BUILD_TYPE}/${CMAKE_GENERATOR_PLATFORM}/PortLink)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries/${CMAKE_BUILD_TYPE}/${CMAKE_GENERATOR_PLATFORM}/PortLink)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries/${CMAKE_BUILD_TYPE}/${CMAKE_GENERATOR_PLATFORM}/PortLink)

# Set intermediate directories
set(CMAKE_INTERMEDIATE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Intermediates/${CMAKE_BUILD_TYPE}/${CMAKE_GENERATOR_PLATFORM}/PortLink)

# Define preprocessor macros and flags for custom configurations
if(CMAKE_BUILD_TYPE STREQUAL "PL_DEBUG")
    add_definitions(-DPL_DEBUG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
elseif(CMAKE_BUILD_TYPE STREQUAL "PL_RELEASE")
    add_definitions(-DPL_RELEASE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
elseif(CMAKE_BUILD_TYPE STREQUAL "PL_DIST")
    add_definitions(-DPL_DIST)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")
else()
    message(FATAL_ERROR "Invalid CMake build type! Please specify one of: PL_DEBUG, PL_RELEASE, or PL_DIST.")
endif()

# Define platform-specific preprocessor macros
if(WIN32)
    add_definitions(-DPL_PLATFORM_WINDOWS)
elseif(UNIX)
    add_definitions(-DPL_PLATFORM_LINUX)
endif()

# Optional: Set platform-specific flags (Windows vs. Linux)
if(MSVC)
    # MSVC-specific flags (e.g., for warnings)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
elseif(APPLE)
    # macOS-specific flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++20")
else()
    # Linux-specific flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++20")
endif()

# Set the allowed configurations for the project
set(CMAKE_CONFIGURATION_TYPES "PL_DEBUG;PL_RELEASE;PL_DIST" CACHE STRING "Supported build types" FORCE)

# Optional: Deployment steps (if you're packaging or installing the project)
# install(TARGETS PortLink DESTINATION bin)
