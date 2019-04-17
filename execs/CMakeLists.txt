#---set paths to find all KVMultiDet headers
KALIVEDA_SET_INCLUDE_DIRS(KVMultiDet MODULES ${KVMultiDet_MOD_LIST})

if(USE_INDRA)
	#---set paths to find all KVIndra headers
	KALIVEDA_SET_INCLUDE_DIRS(KVIndra MODULES ${KVIndra_MOD_LIST})
endif()

set(build_targets
	analyse_logs delete_logs kvdatanalyser kvtreeanalyzer
	KaliVedaAnalysis update_runlist kaliveda kaliveda-sim
        KaliVedaGUI irods_uploader
)
if(USE_INDRA)
    list(APPEND build_targets KVDataBaseGUI test_reconstruction)
endif()
if(USE_INDRAFAZIA AND WITH_MFM)
    list(APPEND build_targets test_indrafazia indrafazia_reconstruction indrafazia_idmaps)
    KALIVEDA_SET_INCLUDE_DIRS(FAZIA MODULES ${FAZIA_MOD_LIST})
endif()

set(test_reconstruction_SRC test_reconstruction.C)
set(test_reconstruction_LIB ${KVIndra_LIB_LIST})

set(test_indrafazia_SRC test_indrafazia.C)
set(test_indrafazia_LIB  ${KVIndra_LIB_LIST} ${FAZIA_LIB_LIST})

set(indrafazia_reconstruction_SRC indrafazia_reconstruction.cpp)
set(indrafazia_reconstruction_LIB  ${KVIndra_LIB_LIST} ${FAZIA_LIB_LIST})
set(indrafazia_idmaps_SRC indrafazia_idmaps.cpp)
set(indrafazia_idmaps_LIB  ${KVIndra_LIB_LIST} ${FAZIA_LIB_LIST})

set(kaliveda_SRC kaliveda.cpp)

set(kaliveda-sim_SRC kaliveda-sim.cpp)

set(analyse_logs_SRC analyse_logs.cxx)

set(delete_logs_SRC delete_logs.cxx)

set(kvdatanalyser_SRC kvdatanalyser.cxx)

set(kvtreeanalyzer_SRC kvtreeanalyzer.cxx)

set(KVDataBaseGUI_SRC runsheetreader.cpp)
set(KVDataBaseGUI_LIB ${KVIndra_LIB_LIST})

set(KaliVedaAnalysis_SRC KaliVedaAnalysis.cpp)

set(KaliVedaGUI_SRC KaliVedaGUI.cpp)

set(update_runlist_SRC update_runlist.cxx)

set(irods_uploader_SRC irods_uploader.cpp)

foreach(t ${build_targets})
	add_executable(${t} ${${t}_SRC})
	target_link_libraries(${t} ${KVMultiDet_LIB_LIST} ${${t}_LIB})
endforeach()

#---install built executables
install(TARGETS ${build_targets} EXPORT ${CMAKE_PROJECT_NAME}Exports RUNTIME DESTINATION bin) 

#---install shell scripts
set(scripts KVA KaliVeda)
install(PROGRAMS ${scripts} DESTINATION bin)
