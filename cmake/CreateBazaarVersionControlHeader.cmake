macro( create_bazaar_version_control_header )

	#---generate (when necessary) bzr version/branch information header KVBzrInfo.h
	#
	# * if Bazaar is not available, this must be a .tgz with an existing KVBzrInfo.h in the source directory,
	#     therefore we copy the existing file
	
	find_package(Bazaar)
	
	if(Bazaar_FOUND)
	
		if(EXISTS ${CMAKE_SOURCE_DIR}/.bzr)
		
			#--- sources are under version control
			
			set(BZR_LAST_REVISION ${CMAKE_SOURCE_DIR}/.bzr/branch/last-revision)
    		
			add_custom_command(
				OUTPUT ${CMAKE_BINARY_DIR}/KVBzrInfo.h
				COMMAND ${CMAKE_SOURCE_DIR}/tools/update_bzrinfo.sh ${Bazaar_EXECUTABLE} ${CMAKE_BINARY_DIR}/KVBzrInfo.h
				DEPENDS ${BZR_LAST_REVISION}
			)
			add_custom_target(bzrinfos DEPENDS ${CMAKE_BINARY_DIR}/KVBzrInfo.h)
			 
		else(EXISTS ${CMAKE_SOURCE_DIR}/.bzr)
			
			#--- sources are not under version control
			 
			if(NOT EXISTS ${CMAKE_SOURCE_DIR}/KVBzrInfo.h) #--- strange: Bazaar is not installed, and no KVBzrInfo.h exists???
			
				message(FATAL_ERROR "not a bzr branch, and ${CMAKE_SOURCE_DIR}/KVBzrInfo.h not found???")
		
			else(NOT EXISTS ${CMAKE_SOURCE_DIR}/KVBzrInfo.h)
		
				#--- copy existing file to build directory
				configure_file(${CMAKE_SOURCE_DIR}/KVBzrInfo.h ${CMAKE_BINARY_DIR})
		
			endif(NOT EXISTS ${CMAKE_SOURCE_DIR}/KVBzrInfo.h)
			
		endif(EXISTS ${CMAKE_SOURCE_DIR}/.bzr)
		
	else(Bazaar_FOUND) #--- Bazaar is not available: therefore we use the existing file (if there is one)
		
		if(NOT EXISTS ${CMAKE_SOURCE_DIR}/KVBzrInfo.h) #--- strange: Bazaar is not installed, and no KVBzrInfo.h exists???
			
			message(FATAL_ERROR "bzr not found, and ${CMAKE_SOURCE_DIR}/KVBzrInfo.h not found???")
		
		else(NOT EXISTS ${CMAKE_SOURCE_DIR}/KVBzrInfo.h)
		
			#--- copy existing file to build directory
			configure_file(${CMAKE_SOURCE_DIR}/KVBzrInfo.h ${CMAKE_BINARY_DIR})
		
		endif(NOT EXISTS ${CMAKE_SOURCE_DIR}/KVBzrInfo.h)
	
	endif(Bazaar_FOUND)

endmacro( create_bazaar_version_control_header )
