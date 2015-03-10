#---set paths to find all KVIndra headers
KALIVEDA_SET_INCLUDE_DIRS(KVIndra MODULES ${KVIndra_MOD_LIST})

BUILD_KALIVEDA_MODULE(geometry
	PARENT ${KVSUBPROJECT}
	EXTRA_LIBS KVIndraexp_events
)
