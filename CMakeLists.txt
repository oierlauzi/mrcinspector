cmake_minimum_required(VERSION 3.5)

# Project information
project(    mrcinspector
            VERSION 0.1.0 
	        DESCRIPTION "CLI tool for inspecting MRC files")

#Set compiler's options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic")

#Register all source files
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/*.c)

# Add an executable with the above sources
add_executable(mrcinspector ${SOURCES})
target_include_directories(mrcinspector PUBLIC ${PROJECT_SOURCE_DIR}/include)

# Set the installation path
install(TARGETS mrcinspector DESTINATION ${CMAKE_INSTALL_BINDIR})