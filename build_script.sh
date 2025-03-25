#!/bin/bash

echo "Creating build directory..."
mkdir build
cd build || exit

echo "Launching CMake..."
cmake .. -G "Unix Makefiles"

cd client || exit
echo "Building client..."
make

cd ..

cd server || exit
echo "Building server..."
make

echo "Build finished"
