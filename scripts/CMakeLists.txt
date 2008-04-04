# CMakeLists for scripts/

SET(kvishellscripts
	browser/kvi4_run_netscape
	config/kvirc4-config
	helpsearch/kvi4_search_help
)

# Install target
IF(UNIX)
	IF(APPLE)
		INSTALL(TARGETS ${kvishellscripts} RUNTIME DESTINATION ${INSTALL_PREFIX}/Contents/MacOS)
	ELSE()
		# Assume linux
		INSTALL(TARGETS ${kvishellscripts} RUNTIME DESTINATION ${INSTALL_PREFIX}/bin)
	ENDIF()
ENDIF()
