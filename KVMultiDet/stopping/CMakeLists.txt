set(ModuleName stopping)
set(ModDeps base events particles)

set(libName ${KVLIBRARY_NAME}${ModuleName})

include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
KALIVEDA_SET_INCLUDE_DIRS(KVMultiDet MODULES ${ModDeps})
KALIVEDA_SET_MODULE_DEPS(kvdeps KVMultiDet MODULES ${ModDeps})

set(do_not_build KVRangeYanez KVRangeYanezMaterial)
if(USE_RANGE)
	set(do_not_build)
endif(USE_RANGE)
#---generate library & rootmap
if(do_not_build)
	KALIVEDA_LIBRARY(${libName} LIB_EXCLUDE ${do_not_build} DEPENDENCIES ${ROOT_LIBRARIES} ${kvdeps})
else()
	KALIVEDA_LIBRARY(${libName} DEPENDENCIES ${ROOT_LIBRARIES} ${kvdeps} range)
endif()
