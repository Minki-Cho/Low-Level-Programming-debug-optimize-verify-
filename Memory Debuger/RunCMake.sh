#!/usr/bin/env bash

# Default values
BUILD_CONFIG=Debug
BUILD_DIR=build
FORCE_DBGHELP=OFF

# Loop over all arguments
for arg in "$@"
do
    case $arg in
        --config=*)
        BUILD_CONFIG="${arg#*=}"
        shift
        ;;
        --dir=*)
        BUILD_DIR="${arg#*=}"
        shift
        ;;
        --forceDbgHelp=*)
        FORCE_DBGHELP="${arg#*=}"
        shift
        ;;
    esac
done

mkdir -p $BUILD_DIR

echo "cmake -G \"Unix Makefiles\" -DCMAKE_BUILD_TYPE=$BUILD_CONFIG -S . -B$BUILD_DIR -DFORCE_DBGHELP=$FORCE_DBGHELP" 
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=$BUILD_CONFIG -S . -B$BUILD_DIR -DFORCE_DBGHELP=$FORCE_DBGHELP

# echo "cmake --build ./$BUILD_DIR --config $BUILD_CONFIG"
# cmake --build ./$BUILD_DIR --config $BUILD_CONFIG
