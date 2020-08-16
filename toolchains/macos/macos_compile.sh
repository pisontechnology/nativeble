#!/bin/bash

realpath() {
  OURPWD=$PWD
  cd "$(dirname "$1")"
  LINK=$(readlink "$(basename "$1")")
  while [ "$LINK" ]; do
    cd "$(dirname "$LINK")"
    LINK=$(readlink "$(basename "$1")")
  done
  REALPATH="$PWD/$(basename "$1")"
  cd "$OURPWD"
  echo "$REALPATH"
}

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
    rm -rf $PROJECT_ROOT/bin/darwin $PROJECT_ROOT/build/darwin
fi

# Compile!
mkdir -p $PROJECT_ROOT/build/darwin
cd $PROJECT_ROOT/build/darwin
cmake -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE -B. -H$PROJECT_ROOT $FLAG_DEBUG
make -j8
cd $PROJECT_ROOT

zip -r -j $PROJECT_ROOT/bin/darwin/darwin-x64.zip $PROJECT_ROOT/bin/darwin