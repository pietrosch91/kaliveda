BUILD_KALIVEDA_MODULE(gui
	PARENT ${KVSUBPROJECT}
	KVMOD_DEPENDS analysis db exp_events daq_cec geometry calibration identification
	EXTRA_LIBS ${KVMultiDet_LIB_LIST}
)
