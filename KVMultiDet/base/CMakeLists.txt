set(extra_libs ${ROOT_Gui_LIBRARY} ${ROOT_Geom_LIBRARY} ${GRU_LIB})
set(lib_exclude)
set(dict_exclude KVError.h)

if(WITH_ZMQ)
    set(extra_libs ${extra_libs} ${ZMQ_LIBRARIES})
    set(dict_exclude ${dict_exclude} KVZMQMessage.h)
else(WITH_ZMQ)
    set(lib_exclude ${lib_exclude} KVZMQMessage)
endif(WITH_ZMQ)

BUILD_KALIVEDA_MODULE(base
	PARENT ${KVSUBPROJECT}
        LIB_EXCLUDE ${lib_exclude}
        DICT_EXCLUDE ${dict_exclude}
        EXTRA_LIBS ${extra_libs}
)

#--- generation of KVBzrInfo.h or KVGitInfo.h
include(CreateVersionControlHeader)
create_version_control_header()

#--- declare dependency of library (in fact, KVBase.o) on KV***Info.h
add_dependencies(KVMultiDetbase vcsinfos)
