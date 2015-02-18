# Source this script to set up the KaliVeda build that this script is part of.
#
# Conveniently an alias like this can be defined in .bashrc:
#   alias thiskaliveda=". bin/thiskaliveda.sh"
#
# This script is for the bash like shells, see thiskaliveda.csh for csh like shells.
#
# Author: Fons Rademakers, 18/8/2006

drop_from_path()
{
   # Assert that we got enough arguments
   if test $# -ne 2 ; then
      echo "drop_from_path: needs 2 arguments"
      return 1
   fi

   p=$1
   drop=$2

   newpath=`echo $p | sed -e "s;:${drop}:;:;g" \
                          -e "s;:${drop};;g"   \
                          -e "s;${drop}:;;g"   \
                          -e "s;${drop};;g"`
}

if [ -n "${KVROOT}" ] ; then
   old_KVROOT=${KVROOT}
fi

if [ "x${BASH_ARGV[0]}" = "x" ]; then
    if [ ! -f bin/thiskaliveda.sh ]; then
        echo ERROR: must "cd where/kaliveda/is" before calling ". bin/thiskaliveda.sh" for this version of bash!
        KVROOT=; export KVROOT
        return 1
    fi
    KVROOT="$PWD"; export KVROOT
else
    # get param to "."
    thiskaliveda=$(dirname ${BASH_ARGV[0]})
    KVROOT=$(cd ${thiskaliveda}/..;pwd); export KVROOT
fi

if [ -n "${old_KVROOT}" ] ; then
   if [ -n "${PATH}" ]; then
      drop_from_path "$PATH" ${old_KVROOT}/bin
      PATH=$newpath
   fi
   if [ -n "${LD_LIBRARY_PATH}" ]; then
      drop_from_path $LD_LIBRARY_PATH ${old_KVROOT}/lib
      LD_LIBRARY_PATH=$newpath
   fi
fi

if [ -z "${PATH}" ]; then
   PATH=$KVROOT/bin; export PATH
else
   PATH=$KVROOT/bin:$PATH; export PATH
fi

if [ -z "${LD_LIBRARY_PATH}" ]; then
   LD_LIBRARY_PATH=$KVROOT/lib; export LD_LIBRARY_PATH       # Linux, ELF HP-UX
else
   LD_LIBRARY_PATH=$KVROOT/lib:$LD_LIBRARY_PATH; export LD_LIBRARY_PATH
fi

unset old_KVROOT
unset thiskaliveda

