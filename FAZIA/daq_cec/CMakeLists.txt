BUILD_KALIVEDA_MODULE(daq_cec
	PARENT ${KVSUBPROJECT}
        KVMOD_DEPENDS protobuf
        EXTRA_LIBS KVMultiDetdaq_cec
)
