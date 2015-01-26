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
#---KALIVEDA_LIBRARY(libname [EXCLUDE toto.h titi.h ...] DEPENDENCIES lib1 lib2)
#---------------------------------------------------------------------------------------------------
function(KALIVEDA_LIBRARY libname)
  CMAKE_PARSE_ARGUMENTS(ARG "" "" "EXCLUDE;DEPENDENCIES" ${ARGN})
  
  #---remove any headers given to EXCLUDE from inc/*.h list
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
  if(ARG_EXCLUDE)
	 list(REMOVE_ITEM headerfiles ${ARG_EXCLUDE})
  endif()
  
  if(${ROOT_VERSION} VERSION_LESS 6)
    ROOT_GENERATE_DICTIONARY(G__${libname} ${headerfiles} LINKDEF LinkDef.h OPTIONS -p)
    ROOT_GENERATE_ROOTMAP(${libname} LINKDEF LinkDef.h DEPENDENCIES ${ARG_DEPENDENCIES})
  else()
    ROOT_GENERATE_DICTIONARY(G__${libname} ${headerfiles} MODULE ${libname} LINKDEF LinkDef.h OPTIONS -p)    
  endif()
  ROOT_LINKER_LIBRARY(${libname} ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp G__${libname}.cxx DEPENDENCIES ${ARG_DEPENDENCIES})
  KALIVEDA_INSTALL_HEADERS()
endfunction()
