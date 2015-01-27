#--- Top-level CMake file for KaliVeda
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(KaliVeda)

find_package(ROOT REQUIRED Hist RIO Tree MathCore Gpad Gui)
message(STATUS "Found ROOT v${ROOT_VERSION}")

include(${ROOT_USE_FILE})
if(NOT DEFINED CMAKE_LIBRARY_OUTPUT_DIRECTORY)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()
link_directories(${ROOT_LIBRARY_DIRS})
include(RootInstallDirs)
# needed for v6: sets cxx11 options (perhaps always needed?)
message(STATUS "ROOT_CXX_FLAGS = ${ROOT_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${ROOT_CXX_FLAGS})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}  ${PROJECT_SOURCE_DIR}/cmake)
include(KaliVedaMacros)

#set(CMAKE_VERBOSE_MAKEFILE ON)

include(CreateKaliVedaVersionHeader)
create_kaliveda_version_header()

include(HandleRootBackwardsCompatibilityIssues)
handle_root_backwards_compatibility_issues()

add_subdirectory(KVMultiDet)
