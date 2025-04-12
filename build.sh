#!/bin/bash

# Directories
INCLUDE_DIR="include"
SRC_DIR="src"
BIN_DIR="bin"
OUTPUT="$BIN_DIR/memory_manager"
DEBUG_OUTPUT="$BIN_DIR/memory_manager_debug"

# Argument: test or source file (e.g., test/foo_test.c or src/main.c)
set -x

SRC_PATH="$1"

# Check if a source file was provided
if [ -z "$SRC_PATH" ]; then
    echo "Usage: $0 <path/to/source_file.c>"
    exit 1
fi

# Check if the file exists
if [ ! -f "$SRC_PATH" ]; then
    echo "Error: Source file not found: $SRC_PATH"
    exit 1
fi

# Create bin/ directory if it doesn't exist
mkdir -p "$BIN_DIR"

# Clean bin/
echo "Cleaning up $BIN_DIR..."
rm -rf "$BIN_DIR"/*

# Common flags
COMMON_FLAGS="-Wall -Wextra -Wimplicit-function-declaration -I$INCLUDE_DIR"

# Compile release version
echo "Compiling release version..."
gcc $COMMON_FLAGS "$SRC_PATH" "$SRC_DIR"/*.c -o "$OUTPUT"

if [ $? -eq 0 ]; then
    echo "Release build successful: $OUTPUT"
else
    echo "Failed to compile release version."
fi

# Compile debug version
echo "Compiling debug version..."
gcc -g $COMMON_FLAGS "$SRC_PATH" "$SRC_DIR"/*.c -o "$DEBUG_OUTPUT"

if [ $? -eq 0 ]; then
    echo "Debug build successful: $DEBUG_OUTPUT"
else
    echo "Failed to compile debug version."
fi
