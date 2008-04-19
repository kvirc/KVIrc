# CMakeLists for doc/
SUBDIRS(scriptexamples)

SET(license COPYING)
SET(howto
	README
	FAQ
	INSTALL
	INSTALL-MacOS.txt
	firefox.hack.txt
	hackers.guide.txt
	scripting.faq.txt
	themes.howto.txt
	translations.howto.txt
	ui-style.tips.txt
	win32.compiling.1.2.txt
)

# User documentation
FILE(GLOB help "${CMAKE_CURRENT_BINARY_DIR}/*.html")

IF(PERL_FOUND)
	ADD_CUSTOM_TARGET(
		docs ALL
	)
	
	ADD_CUSTOM_COMMAND(
		COMMENT "generate the user documentation"
		COMMAND perl ${PROJECT_SOURCE_DIR}/admin/gendoc.pl -v ${VERSION_RELEASE} ${CMAKE_CURRENT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/data/doctemplates/*.template `find ${PROJECT_SOURCE_DIR}/src -name "*.cpp" -print` `find ./ -name "*.h" -print`
		TARGET docs
	)
ENDIF()

# Install target
IF(UNIX)
	IF(APPLE)
		INSTALL(FILES ${license} DESTINATION ${INSTALL_PREFIX}/Contents/Resources/license)
		INSTALL(FILES ${howto} DESTINATION ${INSTALL_PREFIX}/Contents/Resources/doc)
		#INSTALL(FILES ${help} DESTINATION ${INSTALL_PREFIX}/Contents/Resources/pics/)
	ELSE()
		# Assume linux
		INSTALL(FILES ${license} DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/license/)
		INSTALL(FILES ${howto} DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/doc/)
		INSTALL(FILES ${help} DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/help/en/)
	ENDIF()
ELSEIF(WIN32)
	#INSTALL(FILES ${license} DESTINATION ${INSTALL_PREFIX}/ ??? /${VERSION_BRANCH}/license/)
	#INSTALL(FILES ${howto} DESTINATION ${INSTALL_PREFIX}/ ??? /${VERSION_BRANCH}/doc/)
ENDIF()
