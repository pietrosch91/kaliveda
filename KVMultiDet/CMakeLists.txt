set(KVLIBRARY_NAME KVMultiDet)

add_subdirectory(particles)
add_subdirectory(base)

add_dependencies(KVMultiDetparticles KVMultiDetbase)
