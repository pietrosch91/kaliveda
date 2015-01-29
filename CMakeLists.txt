#--- Top-level CMake file for KaliVeda
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(KaliVeda)

find_package(ROOT REQUIRED Geom Spectrum Proof Physics Hist RIO Tree MathCore Matrix Gpad Gui)
message(STATUS "Found ROOT v${ROOT_VERSION}")
#--set cpp flag in KVConfig.h
if(ROOT_opengl_FOUND)
	set(WITH_OPENGL yes)
endif()

include(${ROOT_USE_FILE})
if(NOT DEFINED CMAKE_LIBRARY_OUTPUT_DIRECTORY)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()
link_directories(${ROOT_LIBRARY_DIRS})
include(RootInstallDirs)
# needed for v6: sets cxx11 options (perhaps always needed?)
#message(STATUS "ROOT_CXX_FLAGS = ${ROOT_CXX_FLAGS}")
#set(CMAKE_CXX_FLAGS ${ROOT_CXX_FLAGS})
include_directories(${CMAKE_BINARY_DIR})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}  ${PROJECT_SOURCE_DIR}/cmake)
include(KaliVedaMacros)

#set(CMAKE_VERBOSE_MAKEFILE ON)

#----------------------Build options
include(CMakeDependentOption)

#---GRU Ganil ROOT Utilities for reading GANIL acquisition data
find_package(GRU)
if(GRU_FOUND)
	set(WITH_GRULIB yes) #--- for #define WITH_GRULIB
	include_directories(${GRU_INC_DIR})
	link_directories(${GRU_LIB_DIR})
endif(GRU_FOUND)

#---Ricardo Yanez 'range' tables
option(USE_RANGE
		"Build classes using Ricardo Yanez's 'range' library"
		OFF)
if(USE_RANGE)
	# try to deduce whereabouts from LD_LIBRARY_PATH
	message(STATUS "Using Ricardo Yanez's 'range' library...")
	message(STATUS "   --- looking for librange.so...")
	find_library(RANGE_LIB range HINTS ${RANGE_LIB_DIR} ENV LD_LIBRARY_PATH DOC "I am looking for librange.so...")
	if(NOT RANGE_LIB)
		message(FATAL_ERROR "Can't find 'range' library & header. Use cmake -DRANGE_LIB_DIR=... -DRANGE_INC_DIR=...")
	endif()
	get_filename_component(RANGE_LIBDIR ${RANGE_LIB} DIRECTORY)
	get_filename_component(RANGE_DIR ${RANGE_LIBDIR} DIRECTORY)
	message(STATUS "   --- found in ${RANGE_LIBDIR}...")
	message(STATUS "   --- looking for range.h...")
	find_file(RANGE_INCDIR range.h HINTS ${RANGE_INC_DIR} ${RANGE_DIR}/include)
	if(NOT RANGE_INCDIR)
		message(FATAL_ERROR "Can't find header range.h for ${RANGE_LIBDIR}/librange.so. Use cmake -DRANGE_INC_DIR=")
	endif()
	get_filename_component(RANGE_INCDIR ${RANGE_INCDIR} DIRECTORY)
	message(STATUS "   --- found in ${RANGE_INCDIR}...")
	set(WITH_RANGE_YANEZ yes)
	include_directories(${RANGE_INCDIR})
	link_directories(${RANGE_LIBDIR})
endif(USE_RANGE)
CMAKE_DEPENDENT_OPTION(USE_MODIFIED_RANGE
	"Using modified version of range library with special treatment of gaseous materials..."
	OFF
	"WITH_RANGE_YANEZ"
	OFF
	)
if(USE_MODIFIED_RANGE)
	set(WITH_MODIFIED_RANGE_YANEZ yes)
endif()

#---Set the library version in the main CMakeLists.txt------------------------------------------
file(READ ${CMAKE_SOURCE_DIR}/VERSION versionstr)	
string(STRIP ${versionstr} versionstr)
string(REGEX REPLACE "([0-9]+)[.][0-9]+[/][0-9]+" "\\1" KV_MAJOR_VERSION ${versionstr})
string(REGEX REPLACE "[0-9]+[.]([0-9]+)[/][0-9]+" "\\1" KV_MINOR_VERSION ${versionstr})
string(REGEX REPLACE "[0-9]+[.][0-9]+[/]([0-9]+)" "\\1" KV_PATCH_VERSION ${versionstr})
set(KV_VERSION "${KV_MAJOR_VERSION}.${KV_MINOR_VERSION}.${KV_PATCH_VERSION}")	
set(ROOT_LIBRARY_PROPERTIES ${ROOT_LIBRARY_PROPERTIES}
      VERSION ${KV_VERSION}
      SOVERSION ${KV_MAJOR_VERSION}
		)
		

include(CreateKaliVedaVersionHeader)
create_kaliveda_version_header()

include(HandleRootBackwardsCompatibilityIssues)
handle_root_backwards_compatibility_issues()

add_subdirectory(KVMultiDet)
