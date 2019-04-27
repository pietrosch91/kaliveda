#
# Try to find Ricardo Yanez's librange.so library
# Once done this will define
#
#  RANGE_FOUND    - system has librange.so
#  RANGE_LIB      - full path of librange.so
#  RANGE_LIB_DIR  - directory containing librange.so
#  RANGE_INC_DIR  - path to range.h header
#
# User can suggest location to cmake with: -DRANGE_DIR=...
# This directory should contain both lib/ and include/ directories

message(STATUS "Looking for range library (Ricardo Yanez)...")

# try LD_LIBRARY_PATH to find libRANGE.so
if(RANGE_DIR)
	set(some_hints ${RANGE_DIR}/lib)
endif()
find_library(RANGE_LIB range HINTS ENV LD_LIBRARY_PATH ${some_hints})

if(RANGE_LIB)
	
	get_filename_component(RANGE_LIB_DIR ${RANGE_LIB} DIRECTORY)
	message(STATUS "...found library in ${RANGE_LIB_DIR}")
	#---now find headers
	get_filename_component(searchpath1 ${RANGE_LIB_DIR} DIRECTORY)
	if(RANGE_DIR)
                set(some_hints ${RANGE_DIR}/include/range)
	endif()
        find_file(RANGE_INC_DIR range.h HINTS ${searchpath1}/include/range ${some_hints})
	if(RANGE_INC_DIR)
		
		get_filename_component(RANGE_INC_DIR ${RANGE_INC_DIR} DIRECTORY)
		message(STATUS "...found headers in ${RANGE_INC_DIR}")
		set(RANGE_FOUND TRUE)
		
	else(RANGE_INC_DIR)
	
		message(STATUS "...headers not found")
		set(RANGE_FOUND FALSE)
		
	endif(RANGE_INC_DIR)
	
else(RANGE_LIB)	
	
	message(STATUS "...library not found")
	set(RANGE_FOUND FALSE)
	
endif(RANGE_LIB)	

if(NOT RANGE_FOUND)
	set(RANGE_LIB)
	set(RANGE_LIB_DIR)
	set(RANGE_INC_DIR)
endif()
