set(ModuleName stopping)

set(libName ${KVLIBRARY_NAME}${ModuleName})

include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/KVMultiDet/base
							${CMAKE_SOURCE_DIR}/KVMultiDet/events
							${CMAKE_SOURCE_DIR}/KVMultiDet/particles
							)

#---generate library & rootmap
KALIVEDA_LIBRARY(${libName} LIB_EXCLUDE KVRangeYanez KVRangeYanezMaterial
						DEPENDENCIES ${ROOT_LIBRARIES} KVMultiDetbase KVMultiDetevents KVMultiDetparticles)
