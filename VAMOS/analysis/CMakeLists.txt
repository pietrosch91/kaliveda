if(NOT WITH_BUILTIN_GRU)
	set(excluded_classes KVIVRawDataReconstructor KVIVRawDataAnalyser)
endif()

BUILD_KALIVEDA_MODULE(analysis
	PARENT ${KVSUBPROJECT}
	KVMOD_DEPENDS calibration base exp_events geometry reconstruction identification
	LIB_EXCLUDE ${excluded_classes}
	EXTRA_LIBS ${KVIndra_LIB_LIST} ${KVMultiDet_LIB_LIST}
)
