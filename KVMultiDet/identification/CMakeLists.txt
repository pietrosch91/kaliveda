set(ModuleName identification)
set(ModDeps particles base)

set(libName ${KVLIBRARY_NAME}${ModuleName})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
KALIVEDA_SET_INCLUDE_DIRS(KVMultiDet MODULES ${ModDeps})
KALIVEDA_SET_MODULE_DEPS(kvdeps KVMultiDet MODULES ${ModDeps})

if(NOT WITH_FITLTG)
	set(do_not_compile KVTGIDFitter KVTGIDFunctions KVTGIDGrid KVTGID KVTGIDManager KVTGIDZA KVTGIDZ)
endif()

#---generate library & rootmap
if(do_not_compile)
	KALIVEDA_LIBRARY(${libName} LIB_EXCLUDE ${do_not_compile} DEPENDENCIES ${ROOT_LIBRARIES} ${kvdeps})
else()
	KALIVEDA_LIBRARY(${libName} DEPENDENCIES ${ROOT_LIBRARIES} ${kvdeps})
endif()
