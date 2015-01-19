#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_INIT([fitLTG], [0.1], [frankland@ganil.fr])
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CC
#limit Fortran compiler to either g77 or gfortran (avoid abisoft f77)
AC_PROG_F77([gfortran g77])
AC_PROG_LIBTOOL

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC

# Checks for typedefs, structures, and compiler characteristics.
AC_F77_LIBRARY_LDFLAGS
AC_F77_WRAPPERS
#no optimisation of Fortran77 code
AC_SUBST([FFLAGS], ["-Wall -Wno-unused"])

# Checks for library functions.
AC_CHECK_FUNCS([pow sqrt])
AC_C_CONST

AC_CONFIG_FILES([
   Makefile
   src/Makefile
])

AC_OUTPUT
