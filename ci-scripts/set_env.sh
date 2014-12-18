#!/bin/bash

export REVISION=$(git rev-parse HEAD)
export VERSION="0.0.1"
export PROGRAMNAME="qdevicemonitor"
export OUTPUT_ARCHIVE_NAME="${PROGRAMNAME}-${VERSION}-$(date -u +%Y%m%d-%H-%M-%S)-${REVISION:0:8}"
