#ifeq ($(shell expr $(ROOT_VERSION_CODE) \< $(ROOT_v5_32_00)),1)
# before ROOT 5.32 xrootd libraries were in main ROOT installation
#XRDPOSIX = -L$(ROOTSYS)/lib -lXrdPosix
#else
# since 5.32 xrootd is installed separately in $XRDSYS directory
#XRDPOSIX = -L$(XRDSYS)/lib64 -lXrdPosixPreload
#endif

#---set paths to find all KVMultiDet headers
KALIVEDA_SET_INCLUDE_DIRS(KVMultiDet MODULES ${KVMultiDet_MOD_LIST})

if(USE_INDRA)
	#---set paths to find all KVIndra headers
	KALIVEDA_SET_INCLUDE_DIRS(KVIndra MODULES ${KVIndra_MOD_LIST})
endif()

set(build_targets
	analyse_logs delete_logs kvdatanalyser kvtreeanalyzer
	KaliVedaAnalysis update_runlist
)
if(USE_INDRA)
	list(APPEND build_targets KVDataBaseGUI KaliVedaGUI)
endif()

set(analyse_logs_SRC analyse_logs.cxx)

set(delete_logs_SRC delete_logs.cxx)

set(kvdatanalyser_SRC kvdatanalyser.cxx)

set(kvtreeanalyzer_SRC kvtreeanalyzer.cxx)

set(KVDataBaseGUI_SRC runsheetreader.cpp)
set(KVDataBaseGUI_LIB ${KVIndra_LIB_LIST})

set(KaliVedaAnalysis_SRC KaliVedaAnalysis.cpp)

set(KaliVedaGUI_SRC KaliVedaGUI.cpp)
set(KaliVedaGUI_LIB ${KVIndra_LIB_LIST})

set(update_runlist_SRC update_runlist.cxx)

foreach(t ${build_targets})
	add_executable(${t} ${${t}_SRC})
	target_link_libraries(${t} ${KVMultiDet_LIB_LIST} ${${t}_LIB})
endforeach()

#---install built executables
install(TARGETS ${build_targets} RUNTIME DESTINATION bin) 

#---install shell scripts
set(scripts KVA KaliVeda)
install(PROGRAMS ${scripts} DESTINATION bin)
