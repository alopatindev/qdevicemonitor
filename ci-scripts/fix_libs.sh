#!/bin/sh

EXECFILE="$1"

for i in $(otool -L "${EXECFILE}" | egrep "/usr/local/.*libimobiledevice" | awk '{print $1}'); do
    LIB_NAME="$(echo $i | sed 's!.*\/lib\/!!')"
    echo "fixing $LIB_NAME"
    chmod u+w "${EXECFILE}"
    install_name_tool -change "$i" "@executable_path/../lib/${LIB_NAME}" "${EXECFILE}"
    chmod u-w "${EXECFILE}"
done

otool -L "${EXECFILE}"
