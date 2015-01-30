if(NOT WITH_FITLTG)
	set(do_not_compile KVTGIDFitter KVTGIDFunctions KVTGIDGrid KVTGID KVTGIDManager KVTGIDZA KVTGIDZ)
else()
	include_directories(${FitLTG_SOURCE_DIR}/src)
endif()

BUILD_KALIVEDA_MODULE(identification
	PARENT ${KVSUBPROJECT}
	KVMOD_DEPENDS particles base
	LIB_EXCLUDE ${do_not_compile}
	EXTRA_LIBS ${FITLTG_LIB}
)

#set(ModuleName identification)
#set(ModDeps particles base)

#set(libName ${KVLIBRARY_NAME}${ModuleName})

#include_directories(${CMAKE_CURRENT_SOURCE_DIR})
#KALIVEDA_SET_INCLUDE_DIRS(${KVSUBPROJECT} MODULES ${ModDeps})
#KALIVEDA_SET_MODULE_DEPS(kvdeps ${KVSUBPROJECT} MODULES ${ModDeps})

#if(NOT WITH_FITLTG)
#	set(do_not_compile KVTGIDFitter KVTGIDFunctions KVTGIDGrid KVTGID KVTGIDManager KVTGIDZA KVTGIDZ)
#else()
#	include_directories(${FitLTG_SOURCE_DIR}/src)
#endif()

#---generate library & rootmap
#KALIVEDA_LIBRARY(${libName} LIB_EXCLUDE ${do_not_compile} DEPENDENCIES ${ROOT_LIBRARIES} ${kvdeps} ${FITLTG_LIB})
