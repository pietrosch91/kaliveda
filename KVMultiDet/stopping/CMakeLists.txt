if(NOT WITH_RANGE_YANEZ)
	set(do_not_build KVRangeYanez KVRangeYanezMaterial)
endif()

BUILD_KALIVEDA_MODULE(stopping
	PARENT ${KVSUBPROJECT}
	KVMOD_DEPENDS base events particles
	LIB_EXCLUDE ${do_not_build}
	EXTRA_LIBS ${RANGE_LIB}
)

#set(ModuleName stopping)
#set(ModDeps base events particles)

#set(libName ${KVLIBRARY_NAME}${ModuleName})

#include_directories(${CMAKE_BINARY_DIR})
#include_directories(${CMAKE_CURRENT_SOURCE_DIR})
#KALIVEDA_SET_INCLUDE_DIRS(${KVSUBPROJECT} MODULES ${ModDeps})
#KALIVEDA_SET_MODULE_DEPS(kvdeps ${KVSUBPROJECT} MODULES ${ModDeps})

#if(NOT WITH_RANGE_YANEZ)
#	set(do_not_build KVRangeYanez KVRangeYanezMaterial)
#endif()

#---generate library & rootmap
#KALIVEDA_LIBRARY(${libName} LIB_EXCLUDE ${do_not_build} DEPENDENCIES ${ROOT_LIBRARIES} ${kvdeps} ${RANGE_LIB})
