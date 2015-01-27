include(CMakeParseArguments)

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
    ROOT_GENERATE_ROOTMAP(${libname} LINKDEF LinkDef.h DEPENDENCIES ${ARG_DEPENDENCIES})
  else()
    ROOT_GENERATE_DICTIONARY(G__${libname} ${headerfiles} MODULE ${libname} LINKDEF LinkDef.h OPTIONS -p)    
  endif()
  ROOT_LINKER_LIBRARY(${libname} ${sourcefiles} G__${libname}.cxx DEPENDENCIES ${ARG_DEPENDENCIES})
  KALIVEDA_INSTALL_HEADERS()
endfunction()
