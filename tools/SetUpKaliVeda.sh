###############################################################################
#Bash script for installing KaliVeda on several machines with the
#same account, taking into account different versions of gcc compiler & ROOT,
#and also different versions of KaliVeda.
#This script makes sure all necessary directories exist, and sets the
#environment variables KVROOT, PATH and LD_LIBRARY_PATH.
#
#Put SetUpKaliVeda.sh somewhere in your PATH
#Add the following 'alias' to your .bashrc:
#
#alias SetKaliVedaVersion='. `which SetUpKaliVeda.csh`'
#
#To make sure that your shell is set up correctly to run or install KaliVeda,
#put the following line in your .bashrc:
#
#SetKaliVedaVersion >& /dev/null
#
#You can optionally give a version number and an installation prefix (see below).
#
#If you want to use different versions of KaliVeda in the same session,
#change from one to the other using the command:
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
version=""
prefix=""
need_to_update=0
if [ $# -gt 0 ]; then
   version=$1
   need_to_update=1
   shift
fi
if [ $# -gt 0 ]; then
   prefix=$1
   shift
fi
#
#                   handle version
#
vers_file=$HOME/.kalivedaversion
kvrootver="kvroot"
if [ -z "$version" ]; then
	if [ -s $vers_file ]; then
		version=`cat $vers_file | awk '{ print $1 }'`
      if [ "$version" = "standard" ]; then
         version=""
      fi
      if [ -n "$version" ]; then
	      echo "Using KaliVeda version $version"
         kvrootver="kvroot-$version"
      fi
	fi
else
	echo "Setting KaliVeda version to $version"
   kvrootver="kvroot-$version"
fi
#
#                   handle prefix
#
if [ -z "$prefix" ]; then
	if [ -s $vers_file ]; then
		prefix=`cat $vers_file | awk '{ print $2 }'`
      if [ -n "$prefix" ]; then
	      echo "Using path: $prefix"
      fi
	fi
else
   echo "Setting path : $prefix"
fi
if [ -z "$prefix" ]; then
   prefix=$HOME
   echo "Setting path : $prefix"
fi   
#
#                   store new prefix and version
#
if [ $need_to_update -eq 1 ]; then
   if [ -s $vers_file ]; then
	   rm -f $vers_file
   fi
   if [ -n "$version" ]; then
      echo $version $prefix > $vers_file
   else
      echo "standard $prefix" > $vers_file
   fi
fi
#
#                   get gcc version
#
gccver=`gcc --version | awk '/(GCC)/ {print $3}'`
#
#                   get root version
#
rootver=`root-config --version | sed '/\//s//./g'`
#
#                   create installation directories
#
host_dir_ver="$prefix/.${kvrootver}_gcc-${gccver}_root-${rootver}"
host_dir="$prefix/.${kvrootver}"
if [ ! -d $host_dir_ver ]; then
	mkdir $host_dir_ver
	echo Created new directory $host_dir_ver
fi
if [ ! -d $host_dir ]; then
	mkdir $host_dir
	echo Created new directory $host_dir
fi
if [ ! -d $host_dir/KaliVedaDoc ]; then
	mkdir $host_dir/KaliVedaDoc
fi
if [ ! -h $host_dir_ver/KaliVedaDoc ]; then
	ln -s $host_dir/KaliVedaDoc $host_dir_ver/KaliVedaDoc
fi
if [ ! -d $host_dir/include ]; then
	mkdir $host_dir/include
fi
if [ ! -h $host_dir_ver/include ]; then
	ln -s $host_dir/include $host_dir_ver/include
fi
if [ ! -d $host_dir/src ]; then
	mkdir $host_dir/src
fi
if [ ! -h $host_dir_ver/src ]; then
	ln -s $host_dir/src $host_dir_ver/src
fi
#
#                   set KVROOT
#
KVROOT="$host_dir_ver"; export KVROOT
#
#                   back up original PATH and LD_LIBRARY_PATH
#
if [ -z "$KV_PATH_BACKUP" ]; then
	KV_PATH_BACKUP="$PATH"; export KV_PATH_BACKUP
fi
if [ -z "$KV_LDPATH_BACKUP" ]; then
   if [ -n "$LD_LIBRARY_PATH" ]; then
	   KV_LDPATH_BACKUP="$LD_LIBRARY_PATH"; 
   else
      KV_LDPATH_BACKUP="0"
   fi
   export KV_LDPATH_BACKUP
fi
#
#                   set paths
#
PATH=${KVROOT}/bin:${KV_PATH_BACKUP}; export PATH
if [ $KV_LDPATH_BACKUP = 0 ]; then
   LD_LIBRARY_PATH=${KVROOT}/lib
else
   LD_LIBRARY_PATH=${KVROOT}/lib:${KV_LDPATH_BACKUP}
fi
export LD_LIBRARY_PATH
#
#                   print result
#
echo "KVROOT = $KVROOT"
echo "PATH = $PATH"
echo "LD_LIBRARY_PATH = $LD_LIBRARY_PATH"
