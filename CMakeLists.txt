cmake_minimum_required(VERSION 3.14)
project(OrderMatchingEngine VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find threads package
find_package(Threads REQUIRED)

# Add include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Define source files
set(SOURCES
    src/main.cpp
    src/Order.cpp
    src/OrderBook.cpp
    src/MatchingEngine.cpp
    src/Trade.cpp
)

# Define the executable
add_executable(OrderMatchingEngine ${SOURCES})

# Link with thread library
target_link_libraries(OrderMatchingEngine PRIVATE Threads::Threads)

# Add compiler warnings
if(MSVC)
    target_compile_options(OrderMatchingEngine PRIVATE /W4)
else()
    target_compile_options(OrderMatchingEngine PRIVATE -Wall -Wextra -Wpedantic -O3)
endif()
