if(NOT WITH_FITLTG)
	set(do_not_compile KVIDSiCsI5 KVIDSi75SiLi_e494s KVIDSiCorrCsI KVIDSiLiCsI_e494s)
endif()

BUILD_KALIVEDA_MODULE(identification
	PARENT ${KVSUBPROJECT}
	KVMOD_DEPENDS calibration geometry
	LIB_EXCLUDE ${do_not_compile}
)
