# CMakeLists for scripts/config

# Create bin
if(WANT_COEXISTENCE)
	set(KVIRC_CONFIG ${PACKAGE}${VERSION_MAJOR}-config)
else()
	set(KVIRC_CONFIG ${PACKAGE}-config)
endif()

set(kvirc_config_template "${PROJECT_SOURCE_DIR}/cmake/kvirc-config.cmake")
set(kvirc_config_file "${CMAKE_CURRENT_BINARY_DIR}/${KVIRC_CONFIG}")
if(CMAKE_CONFIGURATION_TYPES)
	set(KVIRC_CONFIG_BUILD_TYPE "$<CONFIG>")
	configure_file("${kvirc_config_template}" "${kvirc_config_file}.in" @ONLY)
	set(kvirc_config_file "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/${KVIRC_CONFIG}")
	file(GENERATE OUTPUT "${kvirc_config_file}" INPUT "${CMAKE_CURRENT_BINARY_DIR}/${KVIRC_CONFIG}.in")
else()
	set(KVIRC_CONFIG_BUILD_TYPE "${CMAKE_BUILD_TYPE}")
	configure_file("${kvirc_config_template}" "${kvirc_config_file}" @ONLY)
endif()

# Install target
if(UNIX)
	if(NOT APPLE)
		# This files are useless because apple bundles are movable
		# Assume linux
		install(FILES "${kvirc_config_file}" DESTINATION ${CMAKE_INSTALL_PREFIX}/bin PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
	endif()
endif()
