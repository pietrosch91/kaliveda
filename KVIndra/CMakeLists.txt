#---list of datasets
file(GLOB datasets RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} INDRA_*)
list(APPEND datasets t10_02)

#---set paths to find all KVMultiDet headers
KALIVEDA_SET_INCLUDE_DIRS(KVMultiDet MODULES ${KVMultiDet_MOD_LIST})

BUILD_KALIVEDA_SUBPROJECT(DATASETS ${datasets}
			IGNORE_DIRS INDRA2ROOT
)
