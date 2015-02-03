macro( create_kaliveda_version_header )
	
	#---generate version information header KVVersion.h
	
	string(TIMESTAMP KV_BUILD_DATE "%Y-%m-%d")
	string(TIMESTAMP KV_BUILD_TIME "%H:%M:%S")
	
	set(KV_USER $ENV{USER})
	
	set(KVPROJ_ROOT_ABS ${CMAKE_SOURCE_DIR})
	
	configure_file(${CMAKE_SOURCE_DIR}/KVVersion.h.in ${CMAKE_BINARY_DIR}/KVVersion.h)

endmacro( create_kaliveda_version_header )
