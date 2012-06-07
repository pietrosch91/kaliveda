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
dnl    KVINCDIR         Where the KaliVeda headers are 
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
  elif test ! x"$KVROOT" = x ; then 
    mykvroot="$KVROOT"
  else
     AC_MSG_ERROR( [Set environment variable KVROOT or use --with-kvroot= to give KaliVeda installation directory] )
  fi
  
    mykvrootlib="$mykvroot/lib"
    mykvrootinc="$mykvroot/bin"
    
   AC_PATH_PROG(KVLIBDIR, libKVMultiDet.so , no, $mykvrootlib)
   
  if test ! "x$KVLIBDIR" = "xno" ; then 
    # define some variables 
    KVLIBDIR=$mykvroot/lib
    KVINCDIR=$mykvroot/include
    AC_SUBST(KVINCDIR)
    AC_SUBST(KVLIBDIR)
    ifelse([$1], , :, [$1])
  else 
    ifelse([$2], , :, [$2])
  fi
])

#
# EOF
#
