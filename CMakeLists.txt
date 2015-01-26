#--- Top-level CMake file for KaliVeda
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(KaliVeda)

find_package(ROOT REQUIRED Hist RIO Tree MathCore Gpad Gui)

include(${ROOT_USE_FILE})
if(NOT DEFINED CMAKE_LIBRARY_OUTPUT_DIRECTORY)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()
link_directories(${ROOT_LIBRARY_DIRS})
include(RootInstallDirs)
# needed for v6: sets cxx11 options (perhaps always needed?)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${ROOT_CXX_FLAGS})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}  ${PROJECT_SOURCE_DIR}/cmake)
include(KaliVedaMacros)

#set(CMAKE_VERBOSE_MAKEFILE ON)

#---generate version information header KVVersion.h
file(READ ${CMAKE_SOURCE_DIR}/VERSION versionstr)
string(STRIP ${versionstr} versionstr)
string(REGEX REPLACE "([0-9]+)[.][0-9]+[/][0-9]+" "\\1" KV_MAJOR_VERSION ${versionstr})
string(REGEX REPLACE "[0-9]+[.]([0-9]+)[/][0-9]+" "\\1" KV_MINOR_VERSION ${versionstr})
string(REGEX REPLACE "[0-9]+[.][0-9]+[/]([0-9]+)" "\\1" KV_PATCH_VERSION ${versionstr})
set(KV_VERSION "${KV_MAJOR_VERSION}.${KV_MINOR_VERSION}.${KV_PATCH_VERSION}")
string(TIMESTAMP KV_BUILD_DATE "%Y-%m-%d")
string(TIMESTAMP KV_BUILD_TIME "%H:%M:%S")
set(KV_USER $ENV{USER})
set(KVPROJ_ROOT_ABS ${CMAKE_SOURCE_DIR})
configure_file(KVVersion.h.in ${CMAKE_BINARY_DIR}/KVVersion.h)

#---generate Bazaar version information header KVBzrInfo.h
#---for the moment, just copy dummy file
configure_file(KVBzrInfo.h ${CMAKE_BINARY_DIR})

add_subdirectory(KVMultiDet)
