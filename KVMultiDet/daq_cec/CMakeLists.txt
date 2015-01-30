set(ModuleName daq_cec)
set(ModDeps base)

set(libName ${KVLIBRARY_NAME}${ModuleName})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
KALIVEDA_SET_INCLUDE_DIRS(KVMultiDet MODULES ${ModDeps})
KALIVEDA_SET_MODULE_DEPS(kvdeps KVMultiDet MODULES ${ModDeps})

if(NOT WITH_BUILTIN_GRU)
	set(excluded_classes GTOneScaler GTScalers GTDataParameters GTGanilData)
endif()

#---generate library & rootmap
KALIVEDA_LIBRARY(${libName} LIB_EXCLUDE ${excluded_classes}
						DEPENDENCIES ${ROOT_LIBRARIES} ${kvdeps} ${BUILTIN_GRU_LIB})
