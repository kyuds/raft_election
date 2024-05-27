cmake_minimum_required(VERSION 3.15)

# Basic setup

project(
    Raft 
    VERSION 0.1
    DESCRIPTION "Raft implementation in C++"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Generate code files from proto files under build/generated
set(PROTO_GEN_DIR "${CMAKE_SOURCE_DIR}/build/generated")
add_subdirectory(proto)

# Include src as subdirectory
add_subdirectory(src)
