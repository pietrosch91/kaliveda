set(ModuleName events)
set(ModDeps base particles)

set(libName ${KVLIBRARY_NAME}${ModuleName})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
KALIVEDA_SET_INCLUDE_DIRS(KVMultiDet MODULES ${ModDeps})
KALIVEDA_SET_MODULE_DEPS(kvdeps KVMultiDet MODULES ${ModDeps})

#---Need ROOT with OpenGL support for KVEventViewer
if(ROOT_opengl_FOUND)
   find_library(ROOT_OPENGL_LIBRARY RGL HINTS ${ROOT_LIBRARY_DIR})
	KALIVEDA_LIBRARY(${libName} DEPENDENCIES ${ROOT_LIBRARIES} ${ROOT_OPENGL_LIBRARY} ${kvdeps})
else()
	#---No OpenGL support : no KVEventViewer
	KALIVEDA_LIBRARY(${libName} LIB_EXCLUDE KVEventViewer DEPENDENCIES ${ROOT_LIBRARIES} ${kvdeps})
endif()
