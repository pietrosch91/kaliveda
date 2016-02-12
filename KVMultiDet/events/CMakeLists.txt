#---Need ROOT with OpenGL support for KVEventViewer
if(ROOT_opengl_FOUND)
	set(extra_lib ${ROOT_RGL_LIBRARY})
else()
	#---No OpenGL support : no KVEventViewer
	set(exclude_class KVEventViewer)
endif()

#---Gemini++ interface
if(WITH_GEMINI)
	set(extra_lib ${extra_lib} ${GEMINI_LIB})
else()
	set(exclude_class ${exclude_class} KVGemini)
endif()

BUILD_KALIVEDA_MODULE(events
	PARENT ${KVSUBPROJECT}
	KVMOD_DEPENDS base particles
	EXTRA_LIBS ${extra_lib}
	LIB_EXCLUDE ${exclude_class}
)
