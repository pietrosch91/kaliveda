#--- Top-level CMake file for KaliVeda
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(KaliVeda)

#--look for ROOT & all essential libraries
find_package(ROOT REQUIRED Rint Graf Geom Spectrum Proof Physics Hist RIO Tree MathCore Matrix Gpad Gui Net)
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

#--- Libraries for INDRA data
option(USE_INDRA "Build libraries for analysis of INDRA data" OFF)

#--- Libraries for INDRA-VAMOS data
option(USE_INDRAVAMOS "Build libraries for analysis of INDRA-VAMOS data" OFF)
if(USE_INDRAVAMOS)
	set(USE_INDRA ON)
endif()

#--- Libraries for FAZIA data
option(USE_FAZIA "Build libraries for analysis of FAZIA data" OFF)
if(USE_FAZIA)
	set(USE_INDRA ON)
endif()
add_feature_info(indra USE_INDRA "Build libraries for analysis of INDRA data")
add_feature_info(indravamos USE_INDRAVAMOS "Build libraries for analysis of INDRA-VAMOS data")
add_feature_info(fazia USE_FAZIA "Build libraries for analysis of FAZIA data")

#----------------------------------------------------------
#------------------END BUILD OPTIONS-----------------------
#----------------------------------------------------------

#------------------Print BUILD OPTIONS-----------------------
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
		

#---configure the KVVersion.h file
include(CreateKaliVedaVersionHeader)
create_kaliveda_version_header()

#---configure the KVConfig.h file
include(CreateConfigHeader)
create_config_header()

add_subdirectory(KVMultiDet)
get_property(KVMultiDet_MOD_LIST GLOBAL PROPERTY KVMultiDet_MOD_LIST)
get_property(KVMultiDet_LIB_LIST GLOBAL PROPERTY KVMultiDet_LIB_LIST)
if(USE_INDRA)
	add_subdirectory(KVIndra)
	get_property(KVIndra_MOD_LIST GLOBAL PROPERTY KVIndra_MOD_LIST)
	get_property(KVIndra_LIB_LIST GLOBAL PROPERTY KVIndra_LIB_LIST)
endif()
if(USE_INDRAVAMOS)
	add_subdirectory(VAMOS)
endif()
if(USE_FAZIA)
	add_subdirectory(FAZIA)
endif()

add_subdirectory(execs)

#---set up list of '.rootrc' config files to be read by KVBase::InitEnvironment
get_property(KALIVEDA_CONF_FILES GLOBAL PROPERTY KALIVEDA_CONF_FILES)
configure_file(etc/config.files.in etc/config.files)
install(FILES ${CMAKE_BINARY_DIR}/etc/config.files DESTINATION etc)
