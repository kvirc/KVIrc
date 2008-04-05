# CMakeLists for scripts/

SET(kvishellscripts
	browser/kvi4_run_netscape
	config/kvirc4-config
	helpsearch/kvi4_search_help
)

# Install target
IF(UNIX)
	IF(APPLE)
		INSTALL(FILES ${kvishellscripts} DESTINATION ${INSTALL_PREFIX}/Contents/MacOS)
	ELSE()
		# Assume linux
		INSTALL(FILES ${kvishellscripts} DESTINATION ${INSTALL_PREFIX}/bin)
	ENDIF()
ENDIF()
