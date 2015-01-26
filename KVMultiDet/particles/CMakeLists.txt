set(ModuleName particles)

set(libName ${KVLIBRARY_NAME}${ModuleName})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/KVMultiDet/base)

#---generate library & rootmap
KALIVEDA_LIBRARY(${libName} DEPENDENCIES ${ROOT_LIBRARIES} KVMultiDetbase)
