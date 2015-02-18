#!/bin/sh

headers=`find $1 -name '*.h' | \grep -v LinkDef.h | \grep -v Template`
for head in $headers; do
   strip_head=`basename $head`
   echo "/usr/include/kaliveda/$strip_head"
done
