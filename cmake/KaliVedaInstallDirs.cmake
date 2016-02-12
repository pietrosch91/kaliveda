# - Define GNU standard installation directories
# Provides install directory variables as defined for GNU software:
#  http://www.gnu.org/prep/standards/html_node/Directory-Variables.html
# Inclusion of this module defines the following variables:
#  CMAKE_INSTALL_<dir>      - destination for files of a given type
#  CMAKE_INSTALL_FULL_<dir> - corresponding absolute path
# where <dir> is one of:
#  BINDIR           - user executables (bin)
#  LIBDIR           - object code libraries (lib or lib64 or lib/<multiarch-tuple> on Debian)
#  INCLUDEDIR       - C/C++ header files (include)
#  SYSCONFDIR       - read-only single-machine data (etc)
#  DATAROOTDIR      - read-only architecture-independent data (share)
#  DATADIR          - read-only architecture-independent data (DATAROOTDIR/kaliveda)
#  MANDIR           - man documentation (DATAROOTDIR/man)
#  MACRODIR         - ROOT macros (DATAROOTDIR/macros)
#  CINTINCDIR       - CINT include files (LIBDIR/cint)
#  ICONDIR          - icons (DATAROOTDIR/icons)
#  SRCDIR           - sources (DATAROOTDIR/src)
#  FONTDIR          - fonts (DATAROOTDIR/fonts)
#  DOCDIR           - documentation kaliveda (DATAROOTDIR/doc/PROJECT_NAME)
#  TESTDIR          - tests (DOCDIR/test)
#  TUTDIR           - tutorials (DOCDIR/tutorials)
#  ACLOCALDIR       - locale-dependent data (DATAROOTDIR/aclocal)
#  CMAKEDIR         - cmake modules (DATAROOTDIR/cmake)
#  ELISPDIR         - lisp files (DATAROOTDIR/emacs/site-lisp)
#
# Each CMAKE_INSTALL_<dir> value may be passed to the DESTINATION options of
# install() commands for the corresponding file type.  If the includer does
# not define a value the above-shown default will be used and the value will
# appear in the cache for editing by the user.
# Each CMAKE_INSTALL_FULL_<dir> value contains an absolute path constructed
# from the corresponding destination by prepending (if necessary) the value
# of CMAKE_INSTALL_PREFIX.

#=============================================================================
# Copyright 2011 Nikita Krupen'ko <krnekit@gmail.com>
# Copyright 2011 Kitware, Inc.
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

set(CMAKE_INSTALL_BINDIR "bin")
set(CMAKE_INSTALL_LIBDIR "lib")
if(gnuinstall)
  set(CMAKE_INSTALL_INCLUDEDIR "include/kaliveda")
else()
  set(CMAKE_INSTALL_INCLUDEDIR "include")
endif()
if(gnuinstall)
  set(CMAKE_INSTALL_DATAROOTDIR "share/kaliveda")
else()
  set(CMAKE_INSTALL_DATAROOTDIR "KVFiles")
endif()
if(gnuinstall)
  set(CMAKE_INSTALL_DOCDIR "share/doc/kaliveda")
else()
  set(CMAKE_INSTALL_DOCDIR ".")
endif()
if(gnuinstall)
  set(CMAKE_INSTALL_SYSCONFDIR "share/kaliveda/etc")
else()
  set(CMAKE_INSTALL_SYSCONFDIR "etc")
endif()
if(gnuinstall)
  set(CMAKE_INSTALL_MANDIR "share/man")
else()
  set(CMAKE_INSTALL_MANDIR "man")
endif()
if(gnuinstall)
  set(CMAKE_INSTALL_SYSCONFROOTDIR "share/kaliveda")
else()
  set(CMAKE_INSTALL_SYSCONFROOTDIR ".")
endif()
set(CMAKE_INSTALL_DATADIR "${CMAKE_INSTALL_DATAROOTDIR}/data")
if(gnuinstall)
  set(CMAKE_INSTALL_CMAKEDIR "${CMAKE_INSTALL_DATAROOTDIR}/cmake")
else()
  set(CMAKE_INSTALL_CMAKEDIR "cmake")
endif()
if(gnuinstall)
  set(CMAKE_INSTALL_TMPLDIR "${CMAKE_INSTALL_DATAROOTDIR}/templates")
else()
  set(CMAKE_INSTALL_TMPLDIR "${CMAKE_INSTALL_DATAROOTDIR}")
endif()
set(CMAKE_INSTALL_TUTDIR "${CMAKE_INSTALL_DOCDIR}/examples")
set(CMAKE_INSTALL_DATASETDIR "${CMAKE_INSTALL_DATAROOTDIR}")
set(CMAKE_INSTALL_CMAKEPKGDIR "${CMAKE_INSTALL_LIBDIR}/kaliveda")


#-----------------------------------------------------------------------------

mark_as_advanced(
  CMAKE_INSTALL_BINDIR
  CMAKE_INSTALL_MANDIR
  CMAKE_INSTALL_LIBDIR
  CMAKE_INSTALL_INCLUDEDIR
  CMAKE_INSTALL_SYSCONFDIR
  CMAKE_INSTALL_DATAROOTDIR
  CMAKE_INSTALL_DATASETDIR
  CMAKE_INSTALL_DATADIR
  CMAKE_INSTALL_DOCDIR
  CMAKE_INSTALL_CMAKEDIR
  CMAKE_INSTALL_CMAKEPKGDIR
  CMAKE_INSTALL_TMPLDIR
  CMAKE_INSTALL_TUTDIR
  )

# Result directories
#
foreach(dir BINDIR
            LIBDIR
            MANDIR
            INCLUDEDIR
            SYSCONFDIR
            DATAROOTDIR
            DATASETDIR
            DATADIR
            DOCDIR
            CMAKEDIR 
            TMPLDIR 
            TUTDIR
            CMAKEPKGDIR )
  if(NOT IS_ABSOLUTE ${CMAKE_INSTALL_${dir}})
    set(CMAKE_INSTALL_FULL_${dir} "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_${dir}}")
  else()
    set(CMAKE_INSTALL_FULL_${dir} "${CMAKE_INSTALL_${dir}}")
  endif()
endforeach()
