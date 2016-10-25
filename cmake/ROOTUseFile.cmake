#---Define the Standard macros for ROOT-----------------------------------------------------------
include(RootNewMacros)
if(NOT DEFINED ROOT_ARCHITECTURE)
  include(CheckCompiler)
endif()

#---Set Link and include directories--------------------------------------------------------------
set(CMAKE_INCLUDE_DIRECTORIES_BEFORE ON)
include_directories(${ROOT_INCLUDE_DIRS})
add_definitions(${ROOT_DEFINITIONS})

if(NOT DEFINED CMAKE_LIBRARY_OUTPUT_DIRECTORY)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()

link_directories(${ROOT_LIBRARY_DIRS})


#---------------------------------------------------------------------------------------------------
#---KALIVEDA_PROVIDED_INSTALL_HEADERS(file1 file2 ... OPTIONS [options])
#---------------------------------------------------------------------------------------------------
function(KALIVEDA_PROVIDED_INSTALL_HEADERS)
  CMAKE_PARSE_ARGUMENTS(ARG "" "" "OPTIONS" ${ARGN})
  install(FILES ${ARG_UNPARSED_ARGUMENTS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
                           COMPONENT dev
                           ${ARG_OPTIONS})
endfunction()


#---------------------------------------------------------------------------------------------------
#---KALIVEDA_GENERATE_DICTIONARY(libname LINKDEF [name of LinkDef.h] HEADERS [toto.h titi.h ...] DEPENDENCIES [lib1 lib2])
#
#---Generate ROOT dictionary & rootmap.
#   This works with ROOT5 or ROOT6.
#   For ROOT6 we generate the .pcm file too.
#   rootmap and pcm will be installed in ${CMAKE_INSTALL_LIBDIR}
#
#---------------------------------------------------------------------------------------------------
function(KALIVEDA_GENERATE_DICTIONARY libname)
  CMAKE_PARSE_ARGUMENTS(ARG "" "LINKDEF" "HEADERS;DEPENDENCIES" ${ARGN})

  if(${ROOT_VERSION} VERSION_LESS 6)
    ROOT_GENERATE_DICTIONARY(G__${libname} ${ARG_HEADERS} LINKDEF ${ARG_LINKDEF} OPTIONS -p)
    ROOT_GENERATE_ROOTMAP(${libname} LINKDEF ${CMAKE_CURRENT_SOURCE_DIR}/${ARG_LINKDEF} DEPENDENCIES ${ARG_DEPENDENCIES})
    set_source_files_properties(${libname}.rootmap PROPERTIES GENERATED TRUE)
  else()
    ROOT_GENERATE_DICTIONARY(G__${libname} ${ARG_HEADERS} MODULE ${libname} LINKDEF ${ARG_LINKDEF} OPTIONS -p)    
    set_source_files_properties(${libname}_rdict.pcm ${libname}.rootmap PROPERTIES GENERATED TRUE)
  endif()
  set_source_files_properties(G__${libname}.cxx G__${libname}.h  PROPERTIES GENERATED TRUE)

endfunction()

#---------------------------------------------------------------------------------------------------
#---KALIVEDA_MAKE_ROOT_LIBRARY(libname [DIRECTORY tata] [DICT_EXCLUDE toto.h titi.h ...] [LIB_EXCLUDE Class1 Class2...] DEPENDENCIES lib1 lib2)
#
#---Generate ROOT dictionary & shared library from all .h & .cpp in current directory
#   or in subdirectory 'tata' if DIRECTORY argument given
#   Directory must also contain LinkDef.h file
#   This works with ROOT5 or ROOT6.
#   For ROOT5 we generate the rootmap. For ROOT6 we generate the .pcm
#
#  To exclude some headers from dictionary generation: DICT_EXCLUDE toto.h titi.h ...
#    - corresponding source file will still be compiled, and header will be installed
#  To exclude some classes from shared library: LIB_EXCLUDE Class1 Class2 ...
#    - corresponding source files will not be compiled
#    - corresponding header files will not be used in dictionary, and not installed
# 
#   All used headers will be installed in ${CMAKE_INSTALL_INCLUDEDIR}
#---------------------------------------------------------------------------------------------------
function(KALIVEDA_MAKE_ROOT_LIBRARY libname)
  CMAKE_PARSE_ARGUMENTS(ARG "" "DIRECTORY" "DICT_EXCLUDE;LIB_EXCLUDE;DEPENDENCIES" ${ARGN})
  
  #---get list of all headers in directory, except LinkDef.h
  set(rheaderfiles)
  if(ARG_DIRECTORY)
   include_directories(${ARG_DIRECTORY})
   file(GLOB files ${ARG_DIRECTORY}/*.h)
   set(LINKDEF_FILE ${ARG_DIRECTORY}/LinkDef.h)
  else(ARG_DIRECTORY)
   file(GLOB files *.h)
   set(LINKDEF_FILE LinkDef.h)
  endif(ARG_DIRECTORY)
  if(files)
    foreach(f ${files})
   	if(NOT f MATCHES LinkDef)
   	  set(rheaderfiles ${rheaderfiles} ${f})
   	endif()
    endforeach()
  endif()
  set(dict_headers)
  #--remove any headers which were given to argument DICT_EXCLUDE
  if(ARG_DICT_EXCLUDE)
    foreach(head ${rheaderfiles})
      get_filename_component(no_dir_head ${head} NAME)
      if(NOT no_dir_head MATCHES ${ARG_DICT_EXCLUDE})
        set(dict_headers ${dict_headers} ${head})
      endif()
    endforeach()
  else()
    set(dict_headers ${rheaderfiles})
  endif()
  #--all header files are installed, even if not used in dictionary generation
  set(inst_headers ${rheaderfiles})
  
  #---get list of all sources in directory
  #---for each class in LIB_EXCLUDE we remove the corresponding .cpp and .h
  #---from source & header file lists
  set(rsourcefiles)
  if(ARG_DIRECTORY)
    file(GLOB rsourcefiles ${ARG_DIRECTORY}/*.cpp ${ARG_DIRECTORY}/*.cxx)
  else(ARG_DIRECTORY)
    file(GLOB rsourcefiles *.cpp *.cxx)
  endif(ARG_DIRECTORY)
  set(sourcefiles)
  if(ARG_LIB_EXCLUDE)
    foreach(source ${rsourcefiles})
      get_filename_component(classname ${source} NAME_WE)
      get_filename_component(path_to_source ${source} PATH)
      if(classname MATCHES ${ARG_LIB_EXCLUDE})
	     list(REMOVE_ITEM dict_headers ${path_to_source}/${classname}.h)
	     list(REMOVE_ITEM inst_headers ${path_to_source}/${classname}.h)
      else()
        set(sourcefiles ${sourcefiles} ${source})
      endif()
    endforeach()
  else()
    set(sourcefiles ${rsourcefiles})
  endif()
  
  KALIVEDA_GENERATE_DICTIONARY(${libname} LINKDEF ${LINKDEF_FILE} HEADERS ${dict_headers} DEPENDENCIES ${ARG_DEPENDENCIES})
  
  ROOT_LINKER_LIBRARY(${libname} ${sourcefiles} G__${libname}.cxx DEPENDENCIES ${ARG_DEPENDENCIES})
  
  KALIVEDA_PROVIDED_INSTALL_HEADERS(${inst_headers})
  
  message(STATUS "dict:${dict_headers}")
  message(STATUS "inst:${inst_headers}")
  
endfunction()
