# This probably doesn't work right now

# -- basic project settings --
cmake_minimum_required(VERSION 3.12.3)	# CMake version check
project(Cave_Story_Remake)		# Create project
set(CMAKE_CXX_STANDARD 17)		# Enable c++17 standard
#set (Cave_Story_Remake_VERSION_MAJOR 0)	# set version number
#set (Cave_Story_Remake_VERSION_MINOR 6)

# -- include directory --
include_directories ("${PROJECT_SOURCE_DIR}/SDL2-2.0.8/include")

set(SOURCE_FILES main.cpp)		# Add main.cpp file of project root directory as source file
add_executable(simple_example ${SOURCE_FILES})       # Add executable target with source files listed in SOURCE_FILES variable