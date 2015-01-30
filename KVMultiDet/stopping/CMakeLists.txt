if(NOT WITH_RANGE_YANEZ)
	set(do_not_build KVRangeYanez KVRangeYanezMaterial)
endif()

BUILD_KALIVEDA_MODULE(stopping
	PARENT ${KVSUBPROJECT}
	KVMOD_DEPENDS base events particles
	LIB_EXCLUDE ${do_not_build}
	EXTRA_LIBS ${RANGE_LIB}
)
