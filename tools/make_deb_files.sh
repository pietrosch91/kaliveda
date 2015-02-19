#!/bin/bash
# After doing:
# cd $2
# cmake $1 -DCMAKE_INSTALL_PREFIX=/usr -Dgnuinstall=yes
# make -j4 install DESTDIR=$2/tmp
#
# $1 = root source directory
# $2 = build directory
# $3 = subproject name (KVMultiDet, KVIndra, etc)
# $4 = package name (libkaliveda etc.)

stringContain() { [ -z "${2##*$1*}" ]; }

if [ $# -lt 4 ]; then
   echo "`basename $0` [source dir] [build dir] [subproject] [package]"
   exit 0
fi

version=`cat $1/VERSION | sed 's/\//./'`

# headers
source_headers=`find $1/$3 -name '*.h'`
base_source_headers=`basename -a $source_headers`
cd $2/tmp
installed_headers=`find usr/include/kaliveda -name '*.h'`
base_installed_headers=`basename -a $installed_headers`
for header in $base_installed_headers; do
   if stringContain $header "$base_source_headers"; then
      SUBPROJ_HEADERS="$SUBPROJ_HEADERS $header"
   else
      echo "$header not found in $3 source directory";
   fi
done
if [ "$3"="KVMultiDet" ]; then
  SUBPROJ_HEADERS="$SUBPROJ_HEADERS KVConfig.h KVVersion.h"
fi

# libraries
# just the .so symlinks => dev package
devlibs=`find usr/lib -name lib$3'*'.so`
allibs=`find usr/lib -name lib$3'*' | grep -v pcm`
# remove from allibs any lib which is in devlibs
for dlib in $allibs; do
   good="yes"
   for lib in $devlibs; do
      if [ "$dlib" = "$lib" ]; then
         good="no"
      fi
   done
   if [ "x$good" = "xyes" ]; then
      libs="$libs $dlib"
   fi
done

# data
datafiles=`ls $1/$3/data`

# templates
tmplfiles=`ls $1/$3/factory`

# etc
etcfiles=`ls $1/$3/etc`

# cmake helper files
cmakefiles=`find usr/lib -name '*.cmake'`

# write files
cd $1
tool_install_file=$1/debian/$4.install
install_file=$1/debian/lib$4.install
dev_install_file=$1/debian/lib$4-dev.install
tool_dirs_file=$1/debian/$4.dirs
dirs_file=$1/debian/lib$4.dirs
dev_dirs_file=$1/debian/lib$4-dev.dirs
rm -f $tool_install_file $tool_dirs_file $install_file $dev_install_file $dirs_file $dev_dirs_file

for file in $datafiles; do
   echo "/usr/share/kaliveda/data/$file" >> $install_file
done
for tmpl in $tmplfiles; do
   echo "/usr/share/kaliveda/templates/$tmpl" >> $install_file
done
for etc in $etcfiles; do
   echo "/usr/share/kaliveda/etc/$etc" >> $install_file
done
for lib in $devlibs; do
   echo "/$lib" >> $dev_install_file
done
for header in $SUBPROJ_HEADERS; do
   echo "/usr/include/kaliveda/$header" >> $dev_install_file
done
for lib in $libs; do
   echo "/$lib" >> $install_file
done
if [ "$3"="KVMultiDet" ]; then
  extras="/usr/share/kaliveda/etc/config.files /usr/lib/libfitltg.so /usr/lib/libgan_tape.so"
  tools="/usr/bin/update_runlist /usr/bin/kvtreeanalyzer /usr/bin/KaliVeda /usr/bin/kvdatanalyser /usr/bin/KaliVedaAnalysis"
  dev_extras="/usr/share/kaliveda/etc/kaliveda.m4 /usr/share/kaliveda/etc/nedit.cf /usr/bin/kaliveda-config"
  for e in $extras; do
     echo "$e" >> $install_file
  done
  for t in $tools; do
     echo "$t" >> $tool_install_file
  done
  for e in $dev_extras; do
     echo "$e" >> $dev_install_file
  done
  for e in $cmakefiles; do
     echo "/$e" >> $dev_install_file
  done
elif [ "$3"="KVIndra" ]; then
  tools="/usr/bin/KaliVedaGUI /usr/bin/KVDataBaseGUI"
  for t in $tools; do
     echo "$t" >> $tool_install_file
  done
fi

# make '.dirs' files
dirname `cat $install_file` | sort -u > $dirs_file
dirname `cat $dev_install_file` | sort -u > $dev_dirs_file
dirname `cat $tool_install_file` | sort -u > $tool_dirs_file
