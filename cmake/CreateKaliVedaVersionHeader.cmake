macro( create_kaliveda_version_header )
	
	#---generate version information header KVVersion.h
	
	file(READ ${CMAKE_SOURCE_DIR}/VERSION versionstr)
	
	string(STRIP ${versionstr} versionstr)
	string(REGEX REPLACE "([0-9]+)[.][0-9]+[/][0-9]+" "\\1" KV_MAJOR_VERSION ${versionstr})
	string(REGEX REPLACE "[0-9]+[.]([0-9]+)[/][0-9]+" "\\1" KV_MINOR_VERSION ${versionstr})
	string(REGEX REPLACE "[0-9]+[.][0-9]+[/]([0-9]+)" "\\1" KV_PATCH_VERSION ${versionstr})
	set(KV_VERSION "${KV_MAJOR_VERSION}.${KV_MINOR_VERSION}.${KV_PATCH_VERSION}")
	
	string(TIMESTAMP KV_BUILD_DATE "%Y-%m-%d")
	string(TIMESTAMP KV_BUILD_TIME "%H:%M:%S")
	
	set(KV_USER $ENV{USER})
	
	set(KVPROJ_ROOT_ABS ${CMAKE_SOURCE_DIR})
	
	configure_file(${CMAKE_SOURCE_DIR}/KVVersion.h.in ${CMAKE_BINARY_DIR}/KVVersion.h)

endmacro( create_kaliveda_version_header )
