#!/bin/bash

PROGRAMNAME="qdevicemonitor"

mkdir -p "${PROGRAMNAME}/icons"
for i in icons/* ; do
    OUT_NAME=$(echo -n "${i}" | sed 's/\.svg$/.png/')
    convert "${i}" "${PROGRAMNAME}/${OUT_NAME}"
done
