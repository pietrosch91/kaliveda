set(ModuleName geometry)

set(libName ${KVLIBRARY_NAME}${ModuleName})

include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/KVMultiDet/base
							${CMAKE_SOURCE_DIR}/KVMultiDet/particles
							${CMAKE_SOURCE_DIR}/KVMultiDet/events
							${CMAKE_SOURCE_DIR}/KVMultiDet/stopping
							)

#---generate library & rootmap
KALIVEDA_LIBRARY(${libName} DEPENDENCIES ${ROOT_LIBRARIES}
		KVMultiDetevents KVMultiDetstopping KVMultiDetbase KVMultiDetparticles)
