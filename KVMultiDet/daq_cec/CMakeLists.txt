set(ModuleName daq_cec)
set(ModDeps base)

set(libName ${KVLIBRARY_NAME}${ModuleName})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
KALIVEDA_SET_INCLUDE_DIRS(KVMultiDet MODULES ${ModDeps})
KALIVEDA_SET_MODULE_DEPS(kvdeps KVMultiDet MODULES ${ModDeps})

set(excluded_classes GTOneScaler GTScalers GTDataParameters GTGanilData KVGRUTapeGanilReader KVGRUNetClientGanilReader)
if(WITH_GRULIB)
	list(REMOVE_ITEM excluded_classes KVGRUTapeGanilReader)
	set(kvdeps ${kvdeps} ${GRU_LIB})
endif()

#---generate library & rootmap
KALIVEDA_LIBRARY(${libName} LIB_EXCLUDE ${excluded_classes} DEPENDENCIES ${ROOT_LIBRARIES} ${kvdeps})
