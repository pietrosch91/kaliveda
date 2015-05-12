#KaliVeda.par: KaliVeda/PROOF-INF/BUILD.sh KaliVeda/PROOF-INF/SETUP.C
#	@tar zcf $@ KaliVeda

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/KaliVeda.par
      COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/KaliVeda
      COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/KaliVeda ${CMAKE_CURRENT_BINARY_DIR}/KaliVeda
      COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_BINARY_DIR} tar -zcf KaliVeda.par KaliVeda
      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/KaliVeda/PROOF-INF/BUILD.sh  ${CMAKE_CURRENT_SOURCE_DIR}/KaliVeda/PROOF-INF/SETUP.C
      )
      
add_custom_target(proof ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/KaliVeda.par)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/KaliVeda.par DESTINATION ${CMAKE_INSTALL_SYSCONFDIR})
