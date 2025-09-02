#!/usr/bin/env bash

LIBRARY_TYPE=static
BUILD_CONFIG=RelWithDebInfo
BUILD_DIR=build
FORCE_DBGHELP=OFF
PROFILER_STATIC_LIB="OFF"
PROFILER_SHARED_LIB="OFF"

# Function to print the help message
print_help() {
    echo "Usage: $0 [options]"
    echo ""
    echo "Options:"
    echo "  --lib=<static|dynamic|shared>    Set the library type (default: static)"
    echo "  --config=<config>                Set the build configuration (default: RelWithDebInfo)"
    echo "  --dir=<directory>                Set the build directory (default: build)"
    echo "  --forceDbgHelp                   Force debug help (default: OFF)"
    echo "  --static                         Set library type to static"
    echo "  --dynamic                        Set library type to dynamic"
    echo "  --shared                         Set library type to shared"
    echo "  --help                           Print this help message"
}

# Check if no arguments are supplied
if [ $# -eq 0 ]; then
    print_help
    exit 0
fi

# Loop over all arguments
for arg in "$@"
do
    case $arg in
        --lib=*)
        LIBRARY_TYPE="${arg#*=}"
        shift
        ;;
        --config=*)
        BUILD_CONFIG="${arg#*=}"
        shift
        ;;
        --dir=*)
        BUILD_DIR="${arg#*=}"
        shift
        ;;
        --forceDbgHelp)
        FORCE_DBGHELP="ON"
        shift
        ;;
        --static|--Static)
        LIBRARY_TYPE="static"
        shift
        ;;
        --dynamic|--Dynamic)
        LIBRARY_TYPE="dynamic"
        shift
        ;;
        --shared|--Shared)
        LIBRARY_TYPE="shared"
        shift
        ;;
        --help|-h)
        print_help
        exit 0
        ;;
        *)
        echo "Unknown option: $arg"
        print_help
        exit 1
        ;;
    esac
done

# Profiler Implementation
if [ "$LIBRARY_TYPE" == "dynamic" ] ; then
    PROFILER_STATIC_LIB="OFF"
    PROFILER_SHARED_LIB="ON"
fi
if [ "$LIBRARY_TYPE" == "shared" ] ; then
    PROFILER_STATIC_LIB="OFF"
    PROFILER_SHARED_LIB="ON"
fi

if [ "$LIBRARY_TYPE" == "static" ] ; then
    PROFILER_STATIC_LIB="ON"
    PROFILER_SHARED_LIB="OFF"
fi

if [ "$FORCE_DBGHELP" != "ON" ] ; then
    FORCE_DBGHELP="OFF"
fi

mkdir -p $BUILD_DIR
echo "cmake -G \"Unix Makefiles\" -DCMAKE_BUILD_TYPE=$BUILD_CONFIG -DPROFILER_BUILD_STATIC_LIBS=$PROFILER_STATIC_LIB -DPROFILER_BUILD_SHARED_LIBS=$PROFILER_SHARED_LIB -S ./liquidfun/Box2D -B$BUILD_DIR -DFORCE_DBGHELP=$FORCE_DBGHELP" 
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=$BUILD_CONFIG -DPROFILER_BUILD_STATIC_LIBS=$PROFILER_STATIC_LIB -DPROFILER_BUILD_SHARED_LIBS=$PROFILER_SHARED_LIB -S ./liquidfun/Box2D -B$BUILD_DIR -DFORCE_DBGHELP=$FORCE_DBGHELP

echo "cmake --build ./$BUILD_DIR --config $BUILD_CONFIG"
cmake --build ./$BUILD_DIR --config %BUILD_CONFIG%
