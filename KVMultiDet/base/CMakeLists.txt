BUILD_KALIVEDA_MODULE(base
	PARENT ${KVSUBPROJECT}
	DICT_EXCLUDE KVError.h
	EXTRA_LIBS ${GRU_LIB}
)

#set(ModuleName base)

#set(libName ${KVLIBRARY_NAME}${ModuleName})

#include_directories(${CMAKE_CURRENT_SOURCE_DIR})

#---generate library & rootmap
#KALIVEDA_LIBRARY(${libName} DICT_EXCLUDE KVError.h DEPENDENCIES ${ROOT_LIBRARIES} ${GRU_LIB})

#--- generation of KVBzrInfo.h
include(CreateBazaarVersionControlHeader)
create_bazaar_version_control_header()

#--- declare dependency of library (in fact, KVBase.o) on KVBzrInfo.h
add_dependencies(KVMultiDetbase bzrinfos)
