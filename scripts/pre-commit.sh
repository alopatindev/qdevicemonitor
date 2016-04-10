#!/bin/sh

against=HEAD
allownonascii="false"

# Redirect output to stderr.
exec 1>&2

if [ "$allownonascii" != "true" ] &&
    test $(git diff --cached --name-only --diff-filter=A -z $against |
      LC_ALL=C tr -d '[ -~]\0' | wc -c) != 0
then
    cat <<\EOF
Error: Attempt to add a non-ASCII file name.

This can cause problems if you want to work with people on other platforms.

To be portable it is advisable to rename the file.

If you know what you are doing you can disable this check using:
EOF
    exit 1
fi

source ci-scripts/set_env.sh

egrep -r "(TODO|FIXME)" "${PROGRAMNAME}"
echo

set -e

echo "rebuilding..."
cd "${PROGRAMNAME}/tests" && qmake && make -j8 && cd ..
qmake && make -j8 && cd ..

echo "checking travis config..."
travis lint | grep "syntax error" && exit 1

# If there are whitespace errors, print the offending file names and fail.
git diff-index --check --cached $against --
git diff $against
