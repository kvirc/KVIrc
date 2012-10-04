# CMakeLists for scripts/config

# Create bin
if(WANT_COEXISTENCE)
	set(KVIRC_CONFIG ${PACKAGE}${VERSION_MAJOR}-config)
else()
	set(KVIRC_CONFIG ${PACKAGE}-config)
endif()

# Install target
if(UNIX)
	if(NOT APPLE)
		# This files are useless because apple bundles are movable
		# Assume linux
		install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${KVIRC_CONFIG} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
	endif()
endif()
