#--- Top-level CMake file for KaliVeda
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(KaliVeda)

find_package(ROOT REQUIRED Graf Geom Spectrum Proof Physics Hist RIO Tree MathCore Matrix Gpad Gui Net)
message(STATUS "Found ROOT v${ROOT_VERSION}")
#--optional support for OpenGL
if(ROOT_opengl_FOUND)
	set(WITH_OPENGL yes)
   find_library(ROOT_OPENGL_LIBRARY RGL HINTS ${ROOT_LIBRARY_DIR})
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


#----------------------------------------------------------
#----------------------BUILD OPTIONS-----------------------
#----------------------------------------------------------
include(FeatureSummary)
include(CMakeDependentOption)

#---Built-in package for reading GANIL acquisition data
option(USE_BUILTIN_GRU
		"Use built-in package for reading GANIL acquisition data (not MFM format)"
		ON)
add_feature_info(builtin_gru USE_BUILTIN_GRU "Use built-in package for reading GANIL acquisition data (not MFM format)")
if(USE_BUILTIN_GRU)
	set(WITH_BUILTIN_GRU yes)
	add_subdirectory(GanTape)
	include_directories(${BUILTIN_GRU_INC_DIR})
endif()

#---GRU Ganil ROOT Utilities for reading GANIL acquisition data
option(USE_GRU
		"Use GANIL ROOT utilities (GRU) library for reading data (including MFM format)"
		OFF)
add_feature_info(gru USE_GRU "Use GANIL ROOT utilities (GRU) library for reading data (including MFM format)")
if(USE_GRU)
	find_package(GRU)
	if(GRU_FOUND)
		set(WITH_GRULIB yes) #--- for #define WITH_GRULIB
		include_directories(${GRU_INC_DIR})
		link_directories(${GRU_LIB_DIR})
	endif(GRU_FOUND)
endif()

#---Ricardo Yanez 'range' tables
option(USE_RANGE
		"Build classes using Ricardo Yanez's 'range' library"
		OFF)
add_feature_info(range USE_RANGE "Build classes using Ricardo Yanez's 'range' library")
if(USE_RANGE)
	find_package(Range)
	if(RANGE_FOUND)
		set(WITH_RANGE_YANEZ yes)
		include_directories(${RANGE_INC_DIR})
		link_directories(${RANGE_LIB_DIR})
	endif()
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

#--- ID Grid fitting routines provided by Laurent Tassan-Got
option(USE_FITLTG "Use Tassan-Got fits for deltaE-E identification grids" ON)
add_feature_info(fitltg USE_FITLTG "Use Tassan-Got fits for deltaE-E identification grids")
if(USE_FITLTG)
	set(WITH_FITLTG yes)
	add_subdirectory(fitltg-0.1)
	set(FITLTG_LIB fitltg)
endif()
#----------------------------------------------------------
#------------------END BUILD OPTIONS-----------------------
#----------------------------------------------------------

Feature_Summary(WHAT ENABLED_FEATURES DESCRIPTION "------ENABLED FEATURES:")
Feature_Summary(WHAT DISABLED_FEATURES DESCRIPTION "------DISABLED FEATURES:")



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

include(CreateConfigHeader)
create_config_header()

add_subdirectory(KVMultiDet)
