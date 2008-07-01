# CMakeLists for doc/
SUBDIRS(scriptexamples)

SET(license COPYING)
SET(howto
	README
	FAQ
	INSTALL
	INSTALL-MacOS.txt
	INSTALL-Win32.txt
	firefox.hack.txt
	hackers.guide.txt
	scripting.faq.txt
	themes.howto.txt
	translations.howto.txt
	ui-style.tips.txt
)

# User documentation
IF(PERL_FOUND)
	INSTALL(CODE "MESSAGE(\"-- Generating user documentation..\")")
	IF(UNIX)
		IF(APPLE)
			INSTALL(CODE "EXEC_PROGRAM(perl ARGS {PROJECT_SOURCE_DIR}/admin/gendoc.pl -v {VERSION_RELEASE} '\$ENV{DESTDIR}/${CMAKE_INSTALL_PREFIX}/Contents/Resources/help/en/' ${PROJECT_SOURCE_DIR}/data/doctemplates/*.template `find ${PROJECT_SOURCE_DIR}/src -name '*.cpp' -print` `find ${PROJECT_SOURCE_DIR}/src -name '*.h' -print` )")
		ELSE()
			# Assume linux
			INSTALL(CODE "EXEC_PROGRAM(perl ARGS ${PROJECT_SOURCE_DIR}/admin/gendoc.pl -v ${VERSION_RELEASE} '\$ENV{DESTDIR}/${CMAKE_INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/help/en/' ${PROJECT_SOURCE_DIR}/data/doctemplates/*.template `find ${PROJECT_SOURCE_DIR}/src -name '*.cpp' -print` `find ${PROJECT_SOURCE_DIR}/src -name '*.h' -print` )")
		ENDIF()
	ELSEIF(WIN32)
		#INSTALL(CODE "EXEC_PROGRAM(perl ARGS ${PROJECT_SOURCE_DIR}/admin/gendoc.pl -v ${VERSION_RELEASE} '${CMAKE_INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/help/en/' ${PROJECT_SOURCE_DIR}/data/doctemplates/*.template `find ${PROJECT_SOURCE_DIR}/src -name '*.cpp' -print` `find ${PROJECT_SOURCE_DIR}/src -name '*.h' -print` )")
	ENDIF()
ENDIF()

# Install target
IF(UNIX)
	IF(APPLE)
		INSTALL(FILES ${license} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/license)
		INSTALL(FILES ${howto} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/doc)
	ELSE()
		# Assume linux
		INSTALL(FILES ${license} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/license/)
		INSTALL(FILES ${howto} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/doc/)
	ENDIF()
ELSEIF(WIN32)
	INSTALL(FILES ${license} DESTINATION ${CMAKE_INSTALL_PREFIX}/license/)
	INSTALL(FILES ${howto} DESTINATION ${CMAKE_INSTALL_PREFIX}/doc/)
ENDIF()
