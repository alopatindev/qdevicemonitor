#!/bin/bash

ALLOWED_AMOUNT=10

STORAGE_URL="$1"

OUT=$(mktemp /tmp/sftp-XXXXXX)

sftp_connect() {
    sftp -q -o "StrictHostKeyChecking no" "${STORAGE_URL}"
}

for B in $(echo 'ls -1' | sftp_connect | grep "^${PROGRAMNAME}" | sort); do
    echo $B >> $OUT
done

BUILDS_TOTAL=$(wc -l $OUT | awk '{print $1}')
echo "builds total: ${BUILDS_TOTAL}"

BUILDS_TO_REMOVE=$(((BUILDS_TOTAL - ALLOWED_AMOUNT)))
[ ${BUILDS_TO_REMOVE} -lt 0 ] && BUILDS_TO_REMOVE=0

echo "builds to be removed: ${BUILDS_TO_REMOVE}"

if [ ${BUILDS_TO_REMOVE} -gt 0 ]; then
    for B in $(head -n ${BUILDS_TO_REMOVE} $OUT); do
        echo rm $B
    done | sftp_connect
fi

rm -f $OUT
