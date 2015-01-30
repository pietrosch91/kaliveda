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
