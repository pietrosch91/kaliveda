include(CMakeParseArguments)

#---------------------------------------------------------------------------------------------------
#---KALIVEDA_SET_INCLUDE_DIRS(library MODULES mod1 mod2 ...)
#---------------------------------------------------------------------------------------------------
function(KALIVEDA_SET_INCLUDE_DIRS lib)

	CMAKE_PARSE_ARGUMENTS(ARG "" "" "MODULES" ${ARGN})
    
   foreach(mod ${ARG_MODULES})
		include_directories(${CMAKE_SOURCE_DIR}/${lib}/${mod})
   endforeach()

endfunction()

#---------------------------------------------------------------------------------------------------
#---CHANGE_LIST_TO_STRING(mystring list1 list2 list3)
#---------------------------------------------------------------------------------------------------
function(CHANGE_LIST_TO_STRING mystring)

	CMAKE_PARSE_ARGUMENTS(ARG "" "" "" ${ARGN})
	set(mylist ${ARG_UNPARSED_ARGUMENTS})
	set(stringy)
   foreach(l ${mylist})
		set(stringy "${stringy}${l} ")
   endforeach()
	set(${mystring} "${stringy}" PARENT_SCOPE)

endfunction()

#---------------------------------------------------------------------------------------------------
#---KALIVEDA_SET_MODULE_DEPS(<variable> library MODULES mod1 mod2 ...)
#---------------------------------------------------------------------------------------------------
function(KALIVEDA_SET_MODULE_DEPS variable)

	CMAKE_PARSE_ARGUMENTS(ARG "" "" "MODULES" ${ARGN})
    
	set(lib ${ARG_UNPARSED_ARGUMENTS})
	set(modlist)
   foreach(mod ${ARG_MODULES})
		set(modlist ${modlist} ${lib}${mod})
   endforeach()
	set(${variable} ${modlist} PARENT_SCOPE)

endfunction()

#---------------------------------------------------------------------------------------------------
#---KALIVEDA__INSTALL_HEADERS([dir1 dir2 ...] OPTIONS [options])
#---------------------------------------------------------------------------------------------------
function(KALIVEDA_INSTALL_HEADERS)
  CMAKE_PARSE_ARGUMENTS(ARG "" "" "OPTIONS" ${ARGN})
  if( ARG_UNPARSED_ARGUMENTS )
    set(dirs ${ARG_UNPARSED_ARGUMENTS})
  else()
    set(dirs .)
  endif()
  foreach(d ${dirs})
    install(DIRECTORY ${d} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
                           COMPONENT headers
									FILES_MATCHING PATTERN "*.h"
                           PATTERN "doc" EXCLUDE
                           PATTERN ".svn" EXCLUDE
                           REGEX "LinkDef" EXCLUDE
                           ${ARG_OPTIONS})
#    set_property(GLOBAL APPEND PROPERTY ROOT_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/${d})
  endforeach()
endfunction()

#---------------------------------------------------------------------------------------------------
#---KALIVEDA_LIBRARY(libname [DICT_EXCLUDE toto.h titi.h ...] [LIB_EXCLUDE Class1 Class2...] DEPENDENCIES lib1 lib2)
#
#---Generate dictionary & shared library (+rootmap, *.pcm file) from all .h & .cpp in current directory
#
#  To exclude some headers from dictionary generation: DICT_EXCLUDE toto.h titi.h ...
#  To exclude some classes from shared library: LIB_EXCLUDE Class1 Class2 ...
#---------------------------------------------------------------------------------------------------
function(KALIVEDA_LIBRARY libname)
  CMAKE_PARSE_ARGUMENTS(ARG "" "" "DICT_EXCLUDE;LIB_EXCLUDE;DEPENDENCIES" ${ARGN})
  
  #---get list of all headers in directory, except LinkDef.h
  #---remove any headers given to DICT_EXCLUDE from list
  set(headerfiles)
  set(rheaderfiles)
  file(GLOB files *.h)
  if(files)
    foreach(f ${files})
   	if(NOT f MATCHES LinkDef)
   	  set(rheaderfiles ${rheaderfiles} ${f})
   	endif()
    endforeach()
  endif()
  string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" ""  headerfiles "${rheaderfiles}")
  if(ARG_DICT_EXCLUDE)
	 list(REMOVE_ITEM headerfiles ${ARG_DICT_EXCLUDE})
  endif()
  
  #---get list of all sources in directory
  #---for each class in LIB_EXCLUDE we remove the corresponding .cpp and .h
  #---from source & header file lists
  set(sourcefiles)
  set(rsourcefiles)
  file(GLOB rsourcefiles *.cpp)
  string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" ""  sourcefiles "${rsourcefiles}")
  if(ARG_LIB_EXCLUDE)
    foreach(clex ${ARG_LIB_EXCLUDE})
	   list(REMOVE_ITEM sourcefiles ${clex}.cpp)
	   list(REMOVE_ITEM headerfiles ${clex}.h)
    endforeach()
  endif()
  
  if(${ROOT_VERSION} VERSION_LESS 6)
    ROOT_GENERATE_DICTIONARY(G__${libname} ${headerfiles} LINKDEF LinkDef.h OPTIONS -p)
    ROOT_GENERATE_ROOTMAP(${libname} LINKDEF ${CMAKE_CURRENT_SOURCE_DIR}/LinkDef.h DEPENDENCIES ${ARG_DEPENDENCIES})
  else()
    ROOT_GENERATE_DICTIONARY(G__${libname} ${headerfiles} MODULE ${libname} LINKDEF LinkDef.h OPTIONS -p)    
  endif()
  ROOT_LINKER_LIBRARY(${libname} ${sourcefiles} G__${libname}.cxx DEPENDENCIES ${ARG_DEPENDENCIES})
  KALIVEDA_INSTALL_HEADERS()
endfunction()

#---------------------------------------------------------------------------------------------------
#---BUILD_KALIVEDA_MODULE(kvmod PARENT kvtopdir [KVMOD_DEPENDS kvmod1 kvmod2...] [EXTRA_LIBS lib1 lib2...] 
#             [DICT_EXCLUDE toto.h titi.h ...] [LIB_EXCLUDE Class1 Class2...])
#
#---Build the current module, i.e. a subdirectory in KVMultiDet/, KVIndra/, etc.
#   PARENT = name of top-level directory i.e. KVMultiDet
#   KVMOD_DEPENDS = list of modules in same top-level directory that this module depends on
#                   i.e. they must & will be built first
#   EXTRA_LIBS = extra libraries/targets this module depends on
#  To exclude some headers from dictionary generation: DICT_EXCLUDE toto.h titi.h ...
#  To exclude some classes from shared library: LIB_EXCLUDE Class1 Class2 ...
#
#  For each module we fill the following GLOBAL properties:
#        ${PARENT}_MOD_LIST : list of all modules in ${PARENT}
#        ${PARENT}_LIB_LIST : list of all library targets in ${PARENT}
#        KALIVEDA_LIB_LIST  : global list of all library targets
#---------------------------------------------------------------------------------------------------
function(BUILD_KALIVEDA_MODULE kvmod)

  message(STATUS "   ...module ${kvmod}")

  CMAKE_PARSE_ARGUMENTS(ARG "" "PARENT" "KVMOD_DEPENDS;DICT_EXCLUDE;LIB_EXCLUDE;EXTRA_LIBS" ${ARGN})

  set(libName ${ARG_PARENT}${kvmod})
  
  set_property(GLOBAL APPEND PROPERTY ${ARG_PARENT}_MOD_LIST ${kvmod})

  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  KALIVEDA_SET_INCLUDE_DIRS(${ARG_PARENT} MODULES ${ARG_KVMOD_DEPENDS})
  KALIVEDA_SET_MODULE_DEPS(kvdeps ${ARG_PARENT} MODULES ${ARG_KVMOD_DEPENDS})

  #---generate library & rootmap
  KALIVEDA_LIBRARY(${libName} LIB_EXCLUDE ${ARG_LIB_EXCLUDE} DICT_EXCLUDE ${ARG_DICT_EXCLUDE}
						DEPENDENCIES ${ROOT_LIBRARIES} ${kvdeps} ${ARG_EXTRA_LIBS})
  
  set_property(GLOBAL APPEND PROPERTY ${ARG_PARENT}_LIB_LIST ${libName})
  set_property(GLOBAL APPEND PROPERTY KALIVEDA_LIB_LIST ${libName})

endfunction()


#---------------------------------------------------------------------------------------------------
#---BUILD_KALIVEDA_SUBPROJECT([DATASETS ds1 ds2 ...])
#                             [IGNORE_DIRS dir1 dir2 ...])
#
#---CMakeLists.txt for a KaliVeda subproject i.e. KVMultiDet, KVIndra, ...
#---DATASETS = list of dataset subdirectories
#---IGNORE_DIRS = subdirectories to ignore
#
#---------------------------------------------------------------------------------------------------
function(BUILD_KALIVEDA_SUBPROJECT)

  CMAKE_PARSE_ARGUMENTS(ARG "" "" "DATASETS;IGNORE_DIRS" ${ARGN})
	
	#---get name of directory = name of subproject
	get_filename_component(KVSUBPROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)
	message(STATUS "Configuring ${KVSUBPROJECT}...")

	#---get list of modules (=subdirectories)
	file(GLOB module_list RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *)
	
	#---remove dataset dirs, etc.
	set(to_be_removed CMakeLists.txt etc data factory)
	list(REMOVE_ITEM module_list ${to_be_removed} ${ARG_DATASETS} ${ARG_IGNORE_DIRS})
	
	#---configure modules
	foreach(mod ${module_list})
		add_subdirectory(${mod})
	endforeach(mod)

	#---install configuration files
	if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/etc)
		file(GLOB kvrootrc RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/etc etc/*.rootrc)
  		set_property(GLOBAL APPEND PROPERTY KALIVEDA_CONF_FILES ${kvrootrc})
		install(DIRECTORY etc DESTINATION ${CMAKE_INSTALL_SYSCONFROOTDIR})
	endif()

	#---everything in data/ is installed in ${CMAKE_INSTALL_DATADIR}
	if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/data)
		install(DIRECTORY data/ DESTINATION ${CMAKE_INSTALL_DATADIR})
	endif()

	#---everything in factory/ is installed in ${CMAKE_INSTALL_TMPLDIR}
	if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/factory)
		install(DIRECTORY factory/ DESTINATION ${CMAKE_INSTALL_TMPLDIR})
	endif()

	#---install dataset directories in ${CMAKE_INSTALL_DATASETDIR}
	if(ARG_DATASETS)
		foreach(d ${ARG_DATASETS})
			install(DIRECTORY ${d} DESTINATION ${CMAKE_INSTALL_DATASETDIR})
			#---write Makefile for automatic database updating
			file(GLOB contents RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/${d} ${d}/*)
			CHANGE_LIST_TO_STRING(st_contents ${contents})
			file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${d}_Makefile "$(KV_WORK_DIR)/db/${d}/DataBase.root : ${st_contents}\n")
			file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${d}_Makefile "	@echo Database needs update\n")
			install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${d}_Makefile DESTINATION ${CMAKE_INSTALL_DATASETDIR}/${d} RENAME Makefile)
		endforeach()
	endif()

endfunction()
