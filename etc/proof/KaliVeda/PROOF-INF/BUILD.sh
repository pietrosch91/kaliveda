#! /bin/sh
# Install library.
 
if [ "" = "clean" ]; then
   exit 0
fi
rc=$?
echo "rc=$?"
if [ $? != "0" ] ; then
   exit 1
fi

exit 0
