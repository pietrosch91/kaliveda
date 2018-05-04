set(extra_libs KVMultiDetexp_events)

BUILD_KALIVEDA_MODULE(geometry
	PARENT ${KVSUBPROJECT}
   KVMOD_DEPENDS calibration signals protobuf
   EXTRA_LIBS ${extra_libs}
)
