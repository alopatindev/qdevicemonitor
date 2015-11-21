#!/bin/bash

echo test0
export REVISION_FULL="${TRAVIS_COMMIT:-$(git rev-parse HEAD)}"
export REVISION="${REVISION_FULL:0:8}"

echo test1
export VERSION_WITH_BUILD_NUMBER="${VERSION}\\ build\\ ${TRAVIS_BUILD_NUMBER:-0}\\ rev\\ ${REVISION}"
echo test2
echo "${VERSION_WITH_BUILD_NUMBER}"
export PROGRAMNAME="qdevicemonitor"
export OUTPUT_DIRECTORY="${PROGRAMNAME}-$(date -u +%Y%m%d-%H-%M-%S)-${REVISION}"
export STORAGE_PATH="${STORAGE_HOSTNAME}:/home/frs/project/${PROGRAMNAME}/ci"
export STORAGE_USER="sbar"
export STORAGE_USER_AND_PATH="${STORAGE_USER}@${STORAGE_PATH}"
echo test3
