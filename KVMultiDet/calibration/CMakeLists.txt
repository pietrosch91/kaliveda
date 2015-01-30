BUILD_KALIVEDA_MODULE(calibration
	PARENT ${KVSUBPROJECT}
	KVMOD_DEPENDS base geometry particles stopping
)

#set(ModuleName calibration)
#set(ModDeps base geometry particles stopping)

#set(libName ${KVLIBRARY_NAME}${ModuleName})

#include_directories(${CMAKE_CURRENT_SOURCE_DIR})
#KALIVEDA_SET_INCLUDE_DIRS(${KVSUBPROJECT} MODULES ${ModDeps})
#KALIVEDA_SET_MODULE_DEPS(kvdeps ${KVSUBPROJECT} MODULES ${ModDeps})

#---generate library & rootmap
#KALIVEDA_LIBRARY(${libName} DEPENDENCIES ${ROOT_LIBRARIES} ${kvdeps})
