#!/usr/bin/env bash

# Store info about in which git branch, what SHA1 and at what date/time
# we executed make.
# call with arguments:
#  gitinfo.sh ${GIT_EXECUTABLE} ${PROJECT_SOURCE_DIR} ${OUTPUT_FILE}

dir=$2
git=$1
dotgit="$dir/.git"

OUT=gitinfo.tmp

$git --git-dir=$dotgit describe --all > $OUT
$git --git-dir=$dotgit describe --always >> $OUT

echo '#define KV_GIT_BRANCH "'`head -n 1 $OUT | tail -n1`'"' > $3
echo '#define KV_GIT_COMMIT "'`head -n 2 $OUT | tail -n1`'"' >> $3

exit 0
