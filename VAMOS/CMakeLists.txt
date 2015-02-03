#---list of datasets
set(datasets VAMOS_e503)

#---set paths to find all KVMultiDet headers
KALIVEDA_SET_INCLUDE_DIRS(KVMultiDet MODULES ${KVMultiDet_MOD_LIST})
#---set paths to find all KVIndra headers
KALIVEDA_SET_INCLUDE_DIRS(KVIndra MODULES ${KVIndra_MOD_LIST})


BUILD_KALIVEDA_SUBPROJECT(DATASETS ${datasets})
