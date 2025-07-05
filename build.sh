#!/bin/bash

# Create build directory if it doesn't exist
mkdir -p build

# Navigate to build directory
cd build

# Generate build files
cmake ..

# Build the project
cmake --build .

# Return to root directory
cd ..

echo "Build complete. The executable is in the build directory."
