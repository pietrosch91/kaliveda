#
# Try to find Gemini++ (compiled with ROOT support)
# Once done this will define
#
#  GEMINI_FOUND    - system has Gemini++
#  GEMINI_LIB      - full path of libGemini.so
#  GEMINI_LIB_DIR  - directory containing libGemini.so
#  GEMINI_INC_DIR  - path to CNucleus.h header
#
# User can suggest location to cmake with: -DGEMINI_DIR=...
# This directory should contain both lib/ and include/gemini directories

message(STATUS "Looking for Gemini++ library (with ROOT support)...")

# try LD_LIBRARY_PATH to find libGEMINI.so
if(GEMINI_DIR)
	set(some_hints ${GEMINI_DIR}/lib)
endif()
find_library(GEMINI_LIB Gemini HINTS ENV LD_LIBRARY_PATH ${some_hints})

if(GEMINI_LIB)
	
	get_filename_component(GEMINI_LIB_DIR ${GEMINI_LIB} DIRECTORY)
	message(STATUS "...found library in ${GEMINI_LIB_DIR}")
	#---now find headers
	get_filename_component(searchpath1 ${GEMINI_LIB_DIR} DIRECTORY)
	if(GEMINI_DIR)
		set(some_hints ${GEMINI_DIR}/include/gemini)
	endif()
	find_file(GEMINI_INC_DIR CNucleus.h HINTS ${searchpath1}/include/gemini ${some_hints})
	if(GEMINI_INC_DIR)
		
		get_filename_component(GEMINI_INC_DIR ${GEMINI_INC_DIR} DIRECTORY)
		message(STATUS "...found headers in ${GEMINI_INC_DIR}")
		set(GEMINI_FOUND TRUE)
		
	else(GEMINI_INC_DIR)
	
		message(STATUS "...headers not found")
		set(GEMINI_FOUND FALSE)
		
	endif(GEMINI_INC_DIR)
	
else(GEMINI_LIB)	
	
	message(STATUS "...library not found")
	set(GEMINI_FOUND FALSE)
	
endif(GEMINI_LIB)	

if(NOT GEMINI_FOUND)
	set(GEMINI_LIB)
	set(GEMINI_LIB_DIR)
	set(GEMINI_INC_DIR)
endif()
