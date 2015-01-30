BUILD_KALIVEDA_MODULE(db
	PARENT ${KVSUBPROJECT}
	KVMOD_DEPENDS exp_events daq_cec geometry identification calibration
	EXTRA_LIBS ${KVMultiDet_LIB_LIST}
)
