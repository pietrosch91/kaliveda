BUILD_KALIVEDA_MODULE(db
	PARENT ${KVSUBPROJECT}
	KVMOD_DEPENDS geometry
	EXTRA_LIBS KVMultiDetdb KVMultiDetexp_events KVMultiDetdata_management
)
