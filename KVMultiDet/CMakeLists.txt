#---CMakeLists.txt for a KaliVeda subproject i.e. KVMultiDet, KVIndra, ...

#---get name of directory = name of subproject
get_filename_component(KVSUBPROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)
message(STATUS "Configuring ${KVSUBPROJECT}...")

#---get list of modules (=subdirectories)
file(GLOB module_list RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *)
set(to_be_removed CMakeLists.txt etc data factory)
list(REMOVE_ITEM module_list ${to_be_removed})
#---set ${KVSUBPROJECT}_MODULE_LIST in parent (i.e. top-level CMakeLists)
set(${KVSUBPROJECT}_MODULE_LIST ${module_list} PARENT_SCOPE)

#---configure modules
foreach(mod ${module_list})
	add_subdirectory(${mod})
endforeach(mod)

#---install configuration files
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/etc)
	install(DIRECTORY etc DESTINATION .)
endif()

#---everything in data/ is installed in ${CMAKE_INSTALL_PREFIX}/KVFiles/data
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/data)
	install(DIRECTORY data DESTINATION KVFiles)
endif()

#---everything in factory/ is installed in ${CMAKE_INSTALL_PREFIX}/KVFiles
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/factory)
	install(DIRECTORY factory/ DESTINATION KVFiles)
endif()
