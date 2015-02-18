# Source this script to set up the KaliVeda build that this script is part of.
#
# Conveniently an alias like this can be defined in ~/.cshrc:
#   alias thisKaliVeda "source bin/thiskaliveda.sh"
#
# This script if for the csh like shells, see thiskaliveda.sh for bash like shells.
#
# Author: Fons Rademakers, 18/8/2006

if ($?KVROOT) then
   set old_KVROOT="$KVROOT"
endif

# $_ should be source .../thiskaliveda.csh
set ARGS=($_)
if ("$ARGS" != "") then
   set thiskaliveda="`dirname ${ARGS[2]}`"
else
   # But $_ might not be set if the script is source non-interactively.
   # In [t]csh the sourced file is inserted 'in place' inside the
   # outer script, so we need an external source of information
   # either via the current directory or an extra parameter.
   if ( -e thiskaliveda.csh ) then
      set thiskaliveda=${PWD}
   else if ( -e bin/thiskaliveda.csh ) then
      set thiskaliveda=${PWD}/bin
   else if ( "$1" != "" ) then
      if ( -e ${1}/bin/thiskaliveda.csh ) then
         set thiskaliveda=${1}/bin
      else if ( -e ${1}/thiskaliveda.csh ) then
         set thiskaliveda=${1}
      else
         echo "thiskaliveda.csh: ${1} does not contain a KaliVeda installation"
      endif
   else
      echo 'Error: The call to "source where_kaliveda_is/bin/thiskaliveda.csh" can not determine the location of the KaliVeda installation'
      echo "because it was embedded another script (this is an issue specific to csh)."
      echo "Use either:"
      echo "   cd where_kaliveda_is; source bin/thiskaliveda.csh"
      echo "or"
      echo "   source where_kaliveda_is/bin/thiskaliveda.csh where_kaliveda_is"
   endif
endif

if ($?thiskaliveda) then

setenv KVROOT "`(cd ${thiskaliveda}/..;pwd)`"

if ($?old_KVROOT) then
   setenv PATH `echo $PATH | sed -e "s;:$old_KVROOT/bin:;:;g" \
                                 -e "s;:$old_KVROOT/bin;;g"   \
                                 -e "s;$old_KVROOT/bin:;;g"   \
                                 -e "s;$old_KVROOT/bin;;g"`
   if ($?LD_LIBRARY_PATH) then
      setenv LD_LIBRARY_PATH `echo $LD_LIBRARY_PATH | \
                             sed -e "s;:$old_KVROOT/lib:;:;g" \
                                 -e "s;:$old_KVROOT/lib;;g"   \
                                 -e "s;$old_KVROOT/lib:;;g"   \
                                 -e "s;$old_KVROOT/lib;;g"`
   endif
endif

set path = ($KVROOT/bin $path)

if ($?LD_LIBRARY_PATH) then
   setenv LD_LIBRARY_PATH $KVROOT/lib:$LD_LIBRARY_PATH      # Linux, ELF HP-UX
else
   setenv LD_LIBRARY_PATH $KVROOT/lib
endif

endif # if ("$thiskaliveda" != "")

set thiskaliveda=
set old_KVROOT=

