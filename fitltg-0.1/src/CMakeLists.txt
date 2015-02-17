add_library(fitltg SHARED fede.c fit_ede.c fit_ede_f.f)

include_directories(${PROJECT_BINARY_DIR})

install(TARGETS fitltg EXPORT KaliVedaExports LIBRARY DESTINATION lib)

#install(FILES fit_ede.h DESTINATION include COMPONENT headers)

add_executable(testfitltg pgm.c)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(testfitltg fitltg m)
