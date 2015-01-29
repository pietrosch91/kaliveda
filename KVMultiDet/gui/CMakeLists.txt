set(ModuleName gui)
set(ModDeps analysis exp_events geometry data_management db stopping events base identification particles)

set(libName ${KVLIBRARY_NAME}${ModuleName})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
KALIVEDA_SET_INCLUDE_DIRS(KVMultiDet MODULES ${ModDeps})
KALIVEDA_SET_MODULE_DEPS(kvdeps KVMultiDet MODULES ${ModDeps})

#---generate library & rootmap
KALIVEDA_LIBRARY(${libName} LIB_EXCLUDE DEPENDENCIES ${ROOT_LIBRARIES} ${kvdeps})
