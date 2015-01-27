set(ModuleName events)

set(libName ${KVLIBRARY_NAME}${ModuleName})

include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/KVMultiDet/base ${CMAKE_SOURCE_DIR}/KVMultiDet/particles)

#---Need ROOT with OpenGL support for KVEventViewer
if(ROOT_opengl_FOUND)
	add_definitions(-DWITH_OPENGL)
   find_library(ROOT_OPENGL_LIBRARY RGL HINTS ${ROOT_LIBRARY_DIR})
	set(extra_root_libs ${ROOT_OPENGL_LIBRARY})
	KALIVEDA_LIBRARY(${libName} DEPENDENCIES ${ROOT_LIBRARIES} ${extra_root_libs} KVMultiDetbase KVMultiDetparticles)
else()
	#---No OpenGL support : no KVEventViewer
	set(exclude_class KVEventViewer)
	KALIVEDA_LIBRARY(${libName} LIB_EXCLUDE ${exclude_class} DEPENDENCIES ${ROOT_LIBRARIES} KVMultiDetbase KVMultiDetparticles)
endif()
