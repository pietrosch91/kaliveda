#
# Try to find GRU
# Once done this will define
#
#  GRU_FOUND    - system has GRU
#  GRU_LIB      - full path of libGRU.so
#  GRU_LIB_DIR  - directory containing libGRU.so
#  GRU_INC_DIR  - path to GRU headers

message(STATUS "Looking for GRU (Ganil ROOT Utilities)...")
# if thisgru.(c)sh script was used, GRUDIR envvar should be set: try $GRUDIR/lib
# or : try LD_LIBRARY_PATH to find libGRU.so
find_library(GRU_LIB GRU HINTS $ENV{GRUDIR}/lib ENV LD_LIBRARY_PATH)

if(GRU_LIB)
	
	get_filename_component(GRU_LIB_DIR ${GRU_LIB} DIRECTORY)
	message(STATUS "...found library in ${GRU_LIB_DIR}")
	#---now find headers
	get_filename_component(searchpath1 ${GRU_LIB_DIR} DIRECTORY)
	find_file(GRU_INC_DIR GBase.h HINTS ${searchpath1}/include $ENV{GRUDIR}/include)
	if(GRU_INC_DIR)
		
		get_filename_component(GRU_INC_DIR ${GRU_INC_DIR} DIRECTORY)
		message(STATUS "...found headers in ${GRU_INC_DIR}")
		set(GRU_INC_DIR ${GRU_INC_DIR} ${searchpath1}/MFMlib ${searchpath1}/vigru)
		set(GRU_FOUND TRUE)
		
	else(GRU_INC_DIR)
	
		message(STATUS "...headers not found")
		set(GRU_FOUND FALSE)
		
	endif(GRU_INC_DIR)
	
else(GRU_LIB)	
	
	message(STATUS "...library not found")
	set(GRU_FOUND FALSE)
	
endif(GRU_LIB)	

if(NOT GRU_FOUND)
	set(GRU_LIB)
	set(GRU_LIB_DIR)
	set(GRU_INC_DIR)
endif()
