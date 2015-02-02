set(excluded_classes KVFocalPlanVamos)

BUILD_KALIVEDA_MODULE(geometry
	PARENT ${KVSUBPROJECT}
	KVMOD_DEPENDS base
	LIB_EXCLUDE ${excluded_classes}
	EXTRA_LIBS ${KVIndra_LIB_LIST} ${KVMultiDet_LIB_LIST}
)
