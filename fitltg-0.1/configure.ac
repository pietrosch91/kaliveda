#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_INIT([fitLTG], [0.1], [frankland@ganil.fr])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC
#limit Fortran compiler to either g77 or gfortran (avoid abisoft f77)
AC_PROG_F77([g77 gfortran])
AC_PROG_RANLIB

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([limits.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_TYPE_INT64_T
AC_F77_LIBRARY_LDFLAGS
AC_F77_WRAPPERS
#no optimisation of Fortran77 code
AC_SUBST([FFLAGS], ["-Wall -Wno-unused"])

# Checks for library functions.
AC_CHECK_FUNCS([pow])

AC_CONFIG_FILES([
   Makefile
   src/Makefile
])

AC_OUTPUT
