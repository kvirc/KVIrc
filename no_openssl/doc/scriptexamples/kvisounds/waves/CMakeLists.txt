# CMakeLists for doc/scriptexamples/kvisounds/waves/

SET(waves
	bip.au
	cuac.au
	deop.au
	join.au
	kick.au
	login.au
	op.au
	ring.au
	send.au
	transfer.au
)

# Install target
IF(UNIX)
	IF(APPLE)
		INSTALL(FILES ${waves} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/doc/scriptexamples/kvisounds/waves)
	ELSE()
		# Assume linux
		INSTALL(FILES ${waves} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/doc/scriptexamples/kvisounds/waves)
	ENDIF()
ELSEIF(WIN32)
	INSTALL(FILES ${waves} DESTINATION ${CMAKE_INSTALL_PREFIX}/doc/scriptexamples/kvisounds/waves)
ENDIF()
