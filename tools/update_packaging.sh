#!/bin/bash
#---update_packaging.sh [source dir] [build dir]
#
# Update packagin information for KaliVeda
#
#  [source dir] = git source directory
#  [build dir]  = build directory
#
# This will:
#   - update the packaging information in the debian directory
#   - update the debian/changelog file
#   - ask if you want to commit the result

if [ $# -lt 2 ]; then
   echo "`basename $0` [source dir] [build dir]"
   exit 0
fi

# UPDATE PACKAGING
$1/tools/make_deb_files.sh $1 $2
cd $1

# UPDATE debian/changelog
dch -i

# REVIEW CHANGES
git status

# TAG & COMMIT
read -p "Commit updated packaging ? ([y]/n): " reply
reply=${reply:-y}
if [ "x$reply" != "xy" ]; then
   read -p "Revert all uncommitted changes [CHECK FIRST - DANGER!] ? (y/[n]): " reply
   reply=${reply:-n}
   if [ "x$reply" = "xy" ]; then
      git reset --hard HEAD
   fi
   exit 0
fi
git commit -a -m "Packaging update"
read -p "Push to github ? ([y]/n): " reply
reply=${reply:-y}
if [ "x$reply" = "xy" ]; then
   git push
fi

exit $?
