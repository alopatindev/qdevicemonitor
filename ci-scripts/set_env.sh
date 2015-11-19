#!/bin/bash

export REVISION=$(git rev-parse HEAD)
export VERSION="0.0.1"
export PROGRAMNAME="qdevicemonitor"
export OUTPUT_ARCHIVE_NAME="${PROGRAMNAME}-$(date -u +%Y%m%d-%H-%M-%S)-${REVISION:0:8}"
export OUTPUT_FULL_ARCHIVE_NAME="${OUTPUT_ARCHIVE_NAME}.zip"
export STORAGE_PATH="${STORAGE_HOSTNAME}:/home/frs/project/${PROGRAMNAME}/ci"
export STORAGE_USER="sbar"
export STORAGE_USER_AND_PATH="${STORAGE_USER}@${STORAGE_PATH}"
