#!/bin/bash
#--- new_release.sh [version] [source dir] [build dir]
#
# Create a new release of KaliVeda
#
#  [version]    = new version string (i.e. 1.12/03)
#  [source dir] = git source directory
#  [build dir]  = build directory
#
# This will:
#   - update the VERSION file with [version]
#   - update the packaging information in the debian directory
#   - update the debian/changelog file
#   - ask if you want to commit and tag the result

if [ $# -lt 3 ]; then
   echo "`basename $0` [version] [source dir] [build dir]"
   exit 0
fi

# GET MAJOR/MINOR/TWEAK REVISION NUMBERS
version=`echo $1 | sed 's/\//./' | sed 's/\./ /g'`
maj_vers=`echo $version | awk '{ print $1 }'`
min_vers=`echo $version | awk '{ print $2 }'`
declare -i twk_vers
twk_vers=$((10#`echo $version | awk '{ print $3 }'`))
tag_twk_vers=`echo $version | awk '{ print $3 }'`
echo "Making release $1 (major=$maj_vers minor=$min_vers tweak=$twk_vers)"
read -p "OK to proceed ? ([y]/n): " reply
reply=${reply:-y}
if [ "x$reply" != "xy" ]; then
   exit 0
fi

# UPDATE VERSION
echo $1 > $2/VERSION

# UPDATE PACKAGING
$2/tools/make_deb_files.sh $2 $3
cd $2

# UPDATE debian/changelog
dch -r OLD
debian_vers="${maj_vers}.${min_vers}.${twk_vers}-0ubuntu1"
dch -v $debian_vers "New release v$1"

# REVIEW CHANGES
more debian/changelog
git status

# TAG & COMMIT
read -p "Tag and commit this release ? ([y]/n): " reply
reply=${reply:-y}
if [ "x$reply" != "xy" ]; then
   read -p "Revert all uncommitted changes [CHECK FIRST - DANGER!] ? (y/[n]): " reply
   reply=${reply:-n}
   if [ "x$reply" = "xy" ]; then
      git reset --hard HEAD
   fi
   exit 0
fi
# make template file for commit message
t=$(tempfile)
echo "Release v$1" > $t
echo "" >> $t
echo "[give description of release]" >> $t
git commit -a -t $t
# in case of any new commits on github
git pull
git tag -a release-${maj_vers}.${min_vers}.${tag_twk_vers} -m "Release v$1"
read -p "Push to github ? ([y]/n): " reply
reply=${reply:-y}
if [ "x$reply" = "xy" ]; then
   git push
   git push --tags
fi

exit $?
