cmake_minimum_required (VERSION 2.8)

project(GanTape)

set(CMAKE_CXX_FLAGS "-w -fPIC")
set(CMAKE_C_FLAGS "-w -fPIC")

set(BUILTIN_GRU_INC_DIR ${PROJECT_SOURCE_DIR}/include PARENT_SCOPE)
set(BUILTIN_GRU_LIB gan_tape PARENT_SCOPE)

include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB sources sources/*)
add_library(gan_tape SHARED ${sources})

install(TARGETS gan_tape LIBRARY DESTINATION lib)
