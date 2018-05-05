# generate protobuf source from event description
message(STATUS "   ...module protobuf")

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS FzEventSet.proto)

add_library(FAZIAprotobuf SHARED ${PROTO_SRCS})
set_target_properties(FAZIAprotobuf PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)
target_link_libraries(FAZIAprotobuf ${PROTOBUF_LIBRARIES})

install(TARGETS FAZIAprotobuf EXPORT ${CMAKE_PROJECT_NAME}Exports DESTINATION ${CMAKE_INSTALL_LIBDIR})
