if(NOT WITH_BUILTIN_GRU)
	set(excluded_classes GTOneScaler GTScalers GTDataParameters GTGanilData)
endif()

BUILD_KALIVEDA_MODULE(daq_cec
	PARENT ${KVSUBPROJECT}
	KVMOD_DEPENDS base
	LIB_EXCLUDE ${excluded_classes}
	EXTRA_LIBS ${BUILTIN_GRU_LIB}
)

#set(ModuleName daq_cec)
#set(ModDeps base)

#set(libName ${KVLIBRARY_NAME}${ModuleName})

#include_directories(${CMAKE_CURRENT_SOURCE_DIR})
#KALIVEDA_SET_INCLUDE_DIRS(${KVSUBPROJECT} MODULES ${ModDeps})
#KALIVEDA_SET_MODULE_DEPS(kvdeps ${KVSUBPROJECT} MODULES ${ModDeps})

#if(NOT WITH_BUILTIN_GRU)
#	set(excluded_classes GTOneScaler GTScalers GTDataParameters GTGanilData)
#endif()

#---generate library & rootmap
#KALIVEDA_LIBRARY(${libName} LIB_EXCLUDE ${excluded_classes}
#						DEPENDENCIES ${ROOT_LIBRARIES} ${kvdeps} ${BUILTIN_GRU_LIB})
