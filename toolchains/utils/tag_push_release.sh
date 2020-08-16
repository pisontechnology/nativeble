#!/bin/bash

if [ ${EUID} -eq 0 ]; then
  echo "This script must not be executed as root!"
  exit 1
fi

PROJECT_ROOT=$(realpath $(dirname `realpath $0`)/../..)

VERSION_FILE="$PROJECT_ROOT/VERSION"
if [[ ! -f "$VERSION_FILE" ]]; then
    echo "VERSION file not found, aborting."
    exit -1
fi

VERSION=`cat $VERSION_FILE`

GIT_BRANCH=`git rev-parse --abbrev-ref HEAD`
if [[ ! $GIT_BRANCH == "master" ]]; then
   echo "Formal release tags can only be appended to the master branch. Aborting."
   exit -1
fi

git tag v$VERSION
git push origin v$VERSION