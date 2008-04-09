# CMakeLists for doc/scriptexamples/mp3share/

SET(mp3share
	mp3share.kvs
	mp3share_icon.png
)

# Install target
IF(UNIX)
	IF(APPLE)
		INSTALL(FILES ${mp3share} DESTINATION ${INSTALL_PREFIX}/Contents/Resources/doc/scriptexamples/mp3share)
	ELSE()
		# Assume linux
		INSTALL(FILES ${mp3share} DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/doc/scriptexamples/mp3share)
	ENDIF()
ELSEIF(WIN32)
	#INSTALL(FILES ${mp3share} DESTINATION ${INSTALL_PREFIX}/ ??? /${VERSION_BRANCH}/doc/scriptexamples/mp3share)
ENDIF()
