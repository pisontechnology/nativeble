#!/bin/bash

PROJECT_ROOT=$(realpath $(dirname `realpath $0`)/../..)
FLAG_DEBUG="-DDEFINE_DEBUG=OFF"
CMAKE_BUILD_TYPE="Release"

# Parse the received commands
while :; do
    case $1 in
        -c|--clean) FLAG_CLEAN="SET"            
        ;;
        -d|--debug) 
        FLAG_DEBUG="-DDEFINE_DEBUG=ON"
        CMAKE_BUILD_TYPE="Debug"            
        ;;
        *) break
    esac
    shift
done

# Cleanup the existing files
if [[ ! -z "$FLAG_CLEAN" ]]; then
    rm -rf $PROJECT_ROOT/bin/linux $PROJECT_ROOT/build/linux
fi

# Compile!
THREAD_COUNT=$(nproc --all)
mkdir -p $PROJECT_ROOT/build/linux
cd $PROJECT_ROOT/build/linux
cmake -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE -B. -H$PROJECT_ROOT $FLAG_DEBUG
make -j$THREAD_COUNT
cd $PROJECT_ROOT

zip -r -j $PROJECT_ROOT/bin/linux/linux-x64.zip $PROJECT_ROOT/bin/linux