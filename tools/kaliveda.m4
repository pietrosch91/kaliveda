dnl -*- mode: autoconf -*- 
dnl
dnl Autoconf macro to check for existence of KaliVeda on the system
dnl Synopsis:
dnl
dnl  KV_PATH([ACTION-IF-FOUND, [ACTION-IF-NOT-FOUND]])
dnl
dnl We look for libKVMultiDet.so in $KVROOT/lib
dnl $KVROOT can be overridden using --with-kvroot=/my/installation/directory
dnl
dnl Some examples: 
dnl 
dnl    KV_PATH( 
dnl        AC_DEFINE([HAVE_KALIVEDA], [1], [Define to 1 if you have KaliVeda installed])),
dnl        AC_MSG_ERROR( [You must install KaliVeda before trying to build this project] )
dnl    )
dnl 
dnl The macro defines the following substitution variables
dnl
dnl    KVLIBDIR         Where the KaliVeda libraries are 
dnl    KVCORELIBS       KaliVeda libraries to link with 
dnl    KVINCDIR         Where the KaliVeda headers are 
dnl
dnl To compile your code use:
dnl    $(CXX) -I$(KVINCDIR) ...
dnl
dnl To link with KaliVeda libraries use:
dnl    $(LD) ... -L$(KVLIBDIR) $(KVCORELIBS)
dnl
AC_DEFUN([KV_PATH],
[
  AC_ARG_WITH([kvroot],
              [AC_HELP_STRING([--with-kvroot],
			      [top of the KaliVeda installation directory])],
    			      [user_kvroot=$withval],
			      [user_kvroot="none"])
  if test ! x"$user_kvroot" = xnone; then
    mykvroot="$user_kvroot"
    withkvroot=yes
  elif test ! x"$KVROOT" = x ; then 
    mykvroot="$KVROOT"
    withkvroot=yes
  else
    withkvroot=no
  fi
  if test ! x"$withkvroot" = xyes ; then
    AC_PATH_PROG(KVCONF, kaliveda-config, no, $PATH)
  else
    AC_PATH_PROG(KVCONF, kaliveda-config, no, $mykvroot/bin)
  fi
    if test ! x"$KVCONF" = "xno" ; then
      KVLIBDIR=`$KVCONF --libdir`
      KVINCDIR=`$KVCONF --incdir`
      KVCORELIBS=`$KVCONF --libs`
      withkaliveda=yes
    else 
  
  if test ! x"$withkvroot" = "xno" ; then
    mykvrootlib="$mykvroot/lib"
    mykvrootinc="$mykvroot/bin"    
    AC_PATH_PROG(KVLIBDIR, libKVMultiDet.so , no, $mykvrootlib) 
    if test ! "x$KVLIBDIR" = "xno" ; then
      KVLIBDIR=$mykvroot/lib
      KVINCDIR=$mykvroot/include
      KVCORELIBS="-lKVMultiDet"
    else
     AC_MSG_ERROR( [KaliVeda is not installed on this system] )
     withkaliveda=no
    fi
  fi 
  
  if ! test x"$withkaliveda" = "xno" ; then
    AC_SUBST(KVINCDIR)
    AC_SUBST(KVLIBDIR)
    AC_SUBST(KVCORELIBS)
    ifelse([$1], , :, [$1])
  else 
    ifelse([$2], , :, [$2])
  fi

fi
])

#
# EOF
#
