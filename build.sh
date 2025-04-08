#!/bin/bash

# Directories
SRC_DIR="src"
INCLUDE_DIR="include"
BIN_DIR="bin"
OUTPUT="$BIN_DIR/memory_manager"
DEBUG_OUTPUT="$BIN_DIR/memory_manager_debug"

# Create bin directory if it doesn't exist
mkdir -p "$BIN_DIR"

# Clear previous binaries
echo "Cleaning up $BIN_DIR..."
rm -rf "$BIN_DIR"/*

# Compile normal version
echo "Compiling release version..."
gcc -I"$INCLUDE_DIR" "$SRC_DIR"/*.c -o "$OUTPUT"

if [ $? -eq 0 ]; then
    echo "Release build successful: $OUTPUT"
else
    echo "Failed to compile release version."
fi

# Compile debug version
echo "Compiling debug version..."
gcc -g -I"$INCLUDE_DIR" "$SRC_DIR"/*.c -o "$DEBUG_OUTPUT"

if [ $? -eq 0 ]; then
    echo "Debug build successful: $DEBUG_OUTPUT"
else
    echo "Failed to compile debug version."
fi
