BUILD_KALIVEDA_MODULE(trieur
	PARENT ${KVSUBPROJECT}
	KVMOD_DEPENDS base
)

#set(ModuleName trieur)
#set(ModDeps base)

#set(libName ${KVLIBRARY_NAME}${ModuleName})

#include_directories(${CMAKE_CURRENT_SOURCE_DIR})
#KALIVEDA_SET_INCLUDE_DIRS(${KVSUBPROJECT} MODULES ${ModDeps})
#KALIVEDA_SET_MODULE_DEPS(kvdeps ${KVSUBPROJECT} MODULES ${ModDeps})

#---generate library & rootmap
#KALIVEDA_LIBRARY(${libName} DEPENDENCIES ${ROOT_LIBRARIES} ${kvdeps})
