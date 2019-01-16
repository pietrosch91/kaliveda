set(deps calibration signals daq_cec)
if(WITH_PROTOBUF)
    set(deps ${deps} protobuf)
endif(WITH_PROTOBUF)

BUILD_KALIVEDA_MODULE(geometry
	PARENT ${KVSUBPROJECT}
   KVMOD_DEPENDS ${deps}
   EXTRA_LIBS KVMultiDetexp_events
)
