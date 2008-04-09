# CMakeLists for doc/
SUBDIRS(scriptexamples)

SET(license COPYING)
SET(howto
	README
	FAQ
	INSTALL-MacOS.txt
	firefox.hack.txt
	hackers.guide.txt
	scripting.faq.txt
	themes.howto.txt
	translations.howto.txt
	ui-style.tips.txt
	win32.compiling.1.2.txt
)

# Install target
IF(UNIX)
	IF(APPLE)
		INSTALL(FILES ${license} DESTINATION ${INSTALL_PREFIX}/Contents/Resources/license)
		INSTALL(FILES ${howto} DESTINATION ${INSTALL_PREFIX}/Contents/Resources/doc)
	ELSE()
		# Assume linux
		INSTALL(FILES ${license} DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/license/)
		INSTALL(FILES ${howto} DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/doc/)
	ENDIF()
ELSEIF(WIN32)
	#INSTALL(FILES ${license} DESTINATION ${INSTALL_PREFIX}/ ??? /${VERSION_BRANCH}/license/)
	#INSTALL(FILES ${howto} DESTINATION ${INSTALL_PREFIX}/ ??? /${VERSION_BRANCH}/doc/)
ENDIF()
