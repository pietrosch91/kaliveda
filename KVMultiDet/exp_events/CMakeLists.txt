set(ModuleName exp_events)
set(ModDeps analysis globvars daq_cec identification base particles geometry stopping events data_management db)

set(libName ${KVLIBRARY_NAME}${ModuleName})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
KALIVEDA_SET_INCLUDE_DIRS(KVMultiDet MODULES ${ModDeps})
KALIVEDA_SET_MODULE_DEPS(kvdeps KVMultiDet MODULES ${ModDeps})

set(do_not_build KVRawDataAnalyser KVGANILDataReader KVGRUNetClientGanilReader KVGRUTapeGanilReader KVRTGIDManager)
if(WITH_GRULIB)
	list(REMOVE_ITEM do_not_build KVGRUTapeGanilReader)
endif()
if(WITH_BUILTIN_GRU)
	list(REMOVE_ITEM do_not_build KVRawDataAnalyser KVGANILDataReader)
endif()
if(WITH_FITLTG)
	list(REMOVE_ITEM do_not_build KVRTGIDManager)
endif()

#---generate library & rootmap
KALIVEDA_LIBRARY(${libName} LIB_EXCLUDE ${do_not_build} DEPENDENCIES ${ROOT_LIBRARIES} ${kvdeps})
