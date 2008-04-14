# CMakeLists for scripts/config
# Install target
IF(UNIX)
	IF(NOT APPLE)
		# This files are useless because apple bundles are movable
		# Assume linux
		INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/kvirc${VERSION_MAJOR}-config DESTINATION ${INSTALL_PREFIX}/bin PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
	ENDIF()
ENDIF()
