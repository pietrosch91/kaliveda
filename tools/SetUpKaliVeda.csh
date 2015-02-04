#!/bin/csh -f
###############################################################################
#C-shell script for installing KaliVeda on several machines with the
#same account, taking into account different versions of gcc compiler & ROOT,
#and also different versions of KaliVeda.
#This script makes sure all necessary directories exist, and sets the
#environment variables KVROOT, PATH and LD_LIBRARY_PATH.
#
#Put SetUpKaliVeda.csh and SetUpKaliVedaDirectories.sh somewhere in your PATH
#Add the following 'alias' to your .cshrc:
#
#alias SetKaliVedaVersion 'source `which SetUpKaliVeda.csh`'
#
#To make sure that your shell is set up correctly to run or install KaliVeda,
#put the following line in your .cshrc:
#
#SetKaliVedaVersion >& /dev/null
#
#You can optionally give a version number and an installation prefix (see below).
#
#If you want to change versions of KaliVeda during an interactive session:
#
#[user@localhost]> SetKaliVedaVersion [version]
#
###############################################################################
#Use : SetUpKaliVeda.sh [version] [prefix]
#
# with version = KaliVeda version identifier
#                default is to use version defined in previous call to SetUp..
#                or no version identifier if none was defined
#       prefix = directory in which KaliVeda directories will be created
#                default is to use user's home directory, or the last value
#                given in a call to SetUp...
#
###############################################################################
#
#                   read arguments
#
#if ROOT has not been installed, we cannot do anything - exit!
which root-config >& /dev/null
set rootconf=$status
if ( $rootconf != 0 ) then
   echo 'Cannot find root-config script. You cannot use KaliVeda.'
   exit
endif
set version=""
set prefix=""
set need_to_update=0
if ( $#argv ) then
   set version=$1
   set need_to_update=1
   shift
endif
if ( $#argv ) then
   set prefix=$1
   shift
endif
#
#                   handle version
#
set vers_file=$HOME/.kalivedaversion
set kvrootver="kvroot"
if ( $version == "" ) then
	if ( -ef $vers_file && !( -z $vers_file ) ) then
		set version=`cat $vers_file | awk '{ print $1 }'`
      if ( $version == "standard" ) then
         set version=""
      endif
      if ( $version != "" ) then
	      echo "Using KaliVeda version $version"
         set kvrootver="kvroot-$version"
      endif
	endif
else
	echo "Setting KaliVeda version to $version"
   set kvrootver="kvroot-$version"
endif
#
#                   handle prefix
#
if ( $prefix == "" ) then
	if ( -ef $vers_file && !( -z $vers_file ) ) then
		set prefix=`cat $vers_file | awk '{ print $2 }'`
      if ( $prefix != "" ) then
	      echo "Using path : $prefix"
      endif
   endif
else
   echo "Setting path : $prefix"
endif
if( $prefix == "" ) then
   set prefix=$HOME
   echo "Setting path : $prefix"
endif   
#
#                   store new prefix and version
#
if ( $need_to_update == 1 ) then
   if ( $version != "" ) then
      echo $version $prefix >! $vers_file
   else
      echo "standard $prefix" >! $vers_file
   endif
endif
#
#                   get gcc version
#
set gccver=`gcc -v |& awk '/(gcc version)/ {print $3}'`
#
#                   get root version
#
set rootver=`root-config --version | sed '/\//s//./g'`
#
#                   create installation directories if necessary
#
set host_dir_ver="$prefix/.${kvrootver}_gcc-${gccver}_root-${rootver}"
#include machine type if not i386 (for backwards compatibility)
if ( $?MACHTYPE ) then
   if ( "$MACHTYPE" != "i386" ) then
      set host_dir_ver="$prefix/.${kvrootver}_gcc-${gccver}_${MACHTYPE}_root-${rootver}"
   endif
endif
set host_dir="$prefix/.${kvrootver}"
SetUpKaliVedaDirectories.sh $host_dir_ver $host_dir
#
#                   set KVROOT
#
setenv KVROOT "$host_dir_ver"
#
#                   back up original PATH and LD_LIBRARY_PATH
#
if ( $?KV_PATH_BACKUP == 0 ) then
	setenv KV_PATH_BACKUP "$PATH"
endif
if ( $?KV_LDPATH_BACKUP == 0 ) then
   if ( $?LD_LIBRARY_PATH ) then
	   setenv KV_LDPATH_BACKUP "$LD_LIBRARY_PATH"
   else
      setenv KV_LDPATH_BACKUP "0"
   endif
endif
#
#                   set paths
#
setenv PATH ${KVROOT}/bin:${KV_PATH_BACKUP}
if ( "$KV_LDPATH_BACKUP" == "0" ) then
	   setenv LD_LIBRARY_PATH ${KVROOT}/lib
else
	   setenv LD_LIBRARY_PATH ${KVROOT}/lib:${KV_LDPATH_BACKUP}
endif
if ( $?ROOT_INCLUDE_PATH ) then
	   setenv ROOT_INCLUDE_PATH ${KVROOT}/include:${ROOT_INCLUDE_PATH}
else
	   setenv ROOT_INCLUDE_PATH ${KVROOT}/include
endif
#
#                   print result
#
echo "KVROOT = $KVROOT"
echo "PATH = $PATH"
echo "LD_LIBRARY_PATH = $LD_LIBRARY_PATH"
echo "ROOT_INCLUDE_PATH = $ROOT_INCLUDE_PATH"
exit
