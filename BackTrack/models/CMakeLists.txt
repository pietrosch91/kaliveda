#---Need Gemini++ interface for SimpleGeminiModel
if(NOT WITH_GEMINI)
	set(exclude_class ${exclude_class} SimpleGeminiModel_Binned)
endif()

BUILD_KALIVEDA_MODULE(models
	KVMOD_DEPENDS base
	PARENT ${KVSUBPROJECT}
	EXTRA_LIBS ${KVMultiDet_LIB_LIST}
   LIB_EXCLUDE ${exclude_class}
)
