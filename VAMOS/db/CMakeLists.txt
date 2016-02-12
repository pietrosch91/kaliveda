BUILD_KALIVEDA_MODULE(db
	PARENT ${KVSUBPROJECT}
	KVMOD_DEPENDS calibration geometry minimiser identification
	EXTRA_LIBS ${KVIndra_LIB_LIST} ${KVMultiDet_LIB_LIST}
)
