BUILD_KALIVEDA_MODULE(base
	PARENT ${KVSUBPROJECT}
	DICT_EXCLUDE KVError.h
	EXTRA_LIBS ${GRU_LIB}
)

#--- generation of KVBzrInfo.h or KVGitInfo.h
include(CreateVersionControlHeader)
create_version_control_header()

#--- declare dependency of library (in fact, KVBase.o) on KV***Info.h
add_dependencies(KVMultiDetbase vcsinfos)
