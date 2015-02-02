BUILD_KALIVEDA_MODULE(identification
	PARENT ${KVSUBPROJECT}
	KVMOD_DEPENDS base reconstruction geometry calibration
	EXTRA_LIBS ${KVIndra_LIB_LIST} ${KVMultiDet_LIB_LIST}
)
