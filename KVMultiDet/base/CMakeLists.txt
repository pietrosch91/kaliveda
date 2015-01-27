set(ModuleName base)

set(libName ${KVLIBRARY_NAME}${ModuleName})

#-- add ${CMAKE_BINARY_DIR} for KVVersion.h & KVBzrInfo.h
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_BINARY_DIR})

#---generate library & rootmap
KALIVEDA_LIBRARY(${libName} EXCLUDE KVError.h DEPENDENCIES ${ROOT_LIBRARIES})

#--- generation of KVBzrInfo.h
include(CreateBazaarVersionControlHeader)
create_bazaar_version_control_header()

#--- declare dependency of library (in fact, KVBase.o) on KVBzrInfo.h
add_dependencies(${libName} bzrinfos)
