cmake_minimum_required(VERSION 3.15)

# Basic setup

project(
    Raft 
    VERSION 0.1
    DESCRIPTION "raft consensus written in C++"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# fetch external libraries
include(FetchContent)

# logging
set(PLOG_LIB https://github.com/SergiusTheBest/plog/archive/refs/tags/1.1.10.tar.gz)
FetchContent_Declare(plog URL ${PLOG_LIB})
FetchContent_MakeAvailable(plog)

# sqlite
set(SQLITE3_LIB https://github.com/sqlite/sqlite/archive/refs/tags/version-3.45.3.tar.gz)
FetchContent_Declare(sqlite3 URL ${SQLITE3_LIB})
FetchContent_MakeAvailable(sqlite3)

# Include src as subdirectory
add_subdirectory(src)
