#---Need ROOT with OpenGL support for KVEventViewer
if(ROOT_opengl_FOUND)
	set(extra_lib ${ROOT_OPENGL_LIBRARY})
else()
	#---No OpenGL support : no KVEventViewer
	set(exclude_class KVEventViewer)
endif()

BUILD_KALIVEDA_MODULE(events
	PARENT ${KVSUBPROJECT}
	KVMOD_DEPENDS base particles
	EXTRA_LIBS ${extra_lib}
	LIB_EXCLUDE ${exclude_class}
)

#set(ModuleName events)
#set(ModDeps base particles)

#set(libName ${KVLIBRARY_NAME}${ModuleName})

#include_directories(${CMAKE_CURRENT_SOURCE_DIR})
#KALIVEDA_SET_INCLUDE_DIRS(${KVSUBPROJECT} MODULES ${ModDeps})
#KALIVEDA_SET_MODULE_DEPS(kvdeps ${KVSUBPROJECT} MODULES ${ModDeps})

#---Need ROOT with OpenGL support for KVEventViewer
#if(ROOT_opengl_FOUND)
#	KALIVEDA_LIBRARY(${libName} DEPENDENCIES ${ROOT_LIBRARIES} ${ROOT_OPENGL_LIBRARY} ${kvdeps})
#else()
	#---No OpenGL support : no KVEventViewer
#	KALIVEDA_LIBRARY(${libName} LIB_EXCLUDE KVEventViewer DEPENDENCIES ${ROOT_LIBRARIES} ${kvdeps})
#endif()
