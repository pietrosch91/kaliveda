#!/bin/csh -f
###############################################################################
#C-shell script for changing between different versions of ROOT on different machines,
#taking into account changes of (gcc) compiler version.
#Sets the environment variables ROOTSYS, PATH and LD_LIBRARY_PATH.
#
#Put SetUpROOT.csh somewhere in your PATH
#Add the following 'alias' to your .cshrc:
#alias SetROOTVersion='source `which SetUpROOT.csh`'
#
#To make sure that your shell is set up correctly to run ROOT,
#put the following line in your .cshrc:
#
#SetROOTVersion >& /dev/null
#
#If you want to change versions of ROOT during an interactive session:
#
#[user@localhost]> SetROOTVersion [version] [prefix]
#
#       Example:  SetROOTVersion v5.15.06
#
#       if gcc version = 4.0.2 this will set ROOTSYS = $HOME/.root-v5.15.06_gcc-4.0.2
#       if the directory does not exist, it will be created
#
#       Example:  SetROOTVersion v5.15.06 /space/franklan
#
#       if gcc version = 4.0.2 this will set ROOTSYS = /space/franklan/.root-v5.15.06_gcc-4.0.2
#       if the directory does not exist, it will be created
###############################################################################
#
#                   read arguments
#
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
set vers_file=$HOME/.rootsys
set rootver="root"
if ( $version == "" ) then
	if ( -ef $vers_file && !( -z $vers_file ) ) then
		set version=`cat $vers_file | awk '{ print $1 }'`
      if ( $version == "standard" ) then
         set version=""
      endif
      if ( $version != "" ) then
	      echo "Using ROOT version $version"
         set rootver="root-$version"
      endif
	endif
else
	echo "Setting ROOT version to $version"
   set rootver="root-$version"
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
#                   create installation directories if necessary
#
set host_dir_ver="$prefix/.${rootver}_gcc-${gccver}"
#include machine type if not i386 (for backwards compatibility)
if ( $?MACHTYPE ) then
   if ( "$MACHTYPE" != "i386" ) then
      set host_dir_ver="$prefix/.${rootver}_gcc-${gccver}_${MACHTYPE}"
   endif
endif
set host_dir="$prefix/.${rootver}"
SetUpROOTDirectories.sh $host_dir_ver $host_dir
#
#                   set ROOTSYS
#
setenv ROOTSYS "$host_dir_ver"
#
#                   back up original PATH and LD_LIBRARY_PATH
#
if ( $?ROOT_PATH_BACKUP == 0 ) then
	setenv ROOT_PATH_BACKUP "$PATH"
endif
if ( $?ROOT_LDPATH_BACKUP == 0 ) then
   if ( $?LD_LIBRARY_PATH ) then
	   setenv ROOT_LDPATH_BACKUP "$LD_LIBRARY_PATH"
   else
      setenv ROOT_LDPATH_BACKUP "0"
   endif
endif
#
#                   set paths
#
setenv PATH ${ROOTSYS}/bin:${ROOT_PATH_BACKUP}
if ( "$ROOT_LDPATH_BACKUP" == "0" ) then
	   setenv LD_LIBRARY_PATH ${ROOTSYS}/lib
else
	   setenv LD_LIBRARY_PATH ${ROOTSYS}/lib:${ROOT_LDPATH_BACKUP}
endif
#if ROOT has not yet been installed (i.e. if this script was just called to create
#the necessary directories and position the paths prior to compilation of ROOT)
#then exit straight away
if ( !( -e $ROOTSYS/bin/root-config ) ) then
   exit
endif
#if SetKaliVedaVersion has previously been used to set KaliVeda
#paths and environment variables, we need to call it again (with no arguments)
#in order to reset the paths: the KaliVeda-related parts will have been
#erased by our work here.
#N.B. if the new ROOT installation has a different version to the previous one,
#this may create new Kaliveda installation directories!!!
if( $?KV_PATH_BACKUP && -ef $HOME/.kalivedaversion && !( -z $HOME/.kalivedaversion ) ) then
   echo "ROOTSYS = $ROOTSYS"
   unsetenv KV_PATH_BACKUP
   if( $?KV_LDPATH_BACKUP ) then
      unsetenv KV_LDPATH_BACKUP
   endif
   source `which SetUpKaliVeda.csh`
else
   echo "ROOTSYS = $ROOTSYS"
   echo "PATH = $PATH"
   echo "LD_LIBRARY_PATH = $LD_LIBRARY_PATH"
endif
exit
