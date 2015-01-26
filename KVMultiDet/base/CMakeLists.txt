set(ModuleName base)

set(libName ${KVLIBRARY_NAME}${ModuleName})

#-- add ${CMAKE_BINARY_DIR} for KVVersion.h & KVBzrInfo.h
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_BINARY_DIR})

#---generate library & rootmap
KALIVEDA_LIBRARY(${libName} EXCLUDE KVVersion.h KVBzrInfo.h KVError.h DEPENDENCIES ${ROOT_LIBRARIES})

