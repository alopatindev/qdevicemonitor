#!/bin/bash

export REVISION=$(git rev-parse HEAD)
export VERSION="1.0.0"
export VERSION_WITH_BUILD_NUMBER="${VERSION} build ${TRAVIS_BUILD_NUMBER}"
export PROGRAMNAME="qdevicemonitor"
export OUTPUT_DIRECTORY="${PROGRAMNAME}-$(date -u +%Y%m%d-%H-%M-%S)-${REVISION:0:8}"
export STORAGE_PATH="${STORAGE_HOSTNAME}:/home/frs/project/${PROGRAMNAME}/ci"
export STORAGE_USER="sbar"
export STORAGE_USER_AND_PATH="${STORAGE_USER}@${STORAGE_PATH}"
