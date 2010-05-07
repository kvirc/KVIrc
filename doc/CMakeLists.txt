# CMakeLists for doc/

SET(license
	COPYING
	LICENSE-GPLV2
	LICENSE-GPLV3
	LICENSE-OPENSSL
)
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
	SET(USERDOCS_INDEX ${CMAKE_CURRENT_BINARY_DIR}/help/en/index.html)
	ADD_CUSTOM_TARGET("userdocs" ALL)
	ADD_CUSTOM_TARGET(userdocs-en DEPENDS ${USERDOCS_INDEX})
	ADD_DEPENDENCIES(userdocs userdocs-en)

	IF(UNIX)
		ADD_CUSTOM_COMMAND(
			OUTPUT ${USERDOCS_INDEX}
			COMMENT "Generating user documentation.."
			COMMAND ${PERL_EXECUTABLE} ${PROJECT_SOURCE_DIR}/admin/gendoc.pl -v ${VERSION_RELEASE} '${CMAKE_CURRENT_BINARY_DIR}/help/en/' ${PROJECT_SOURCE_DIR}/data/doctemplates/ ${PROJECT_SOURCE_DIR}/src/
		)
		IF(APPLE)
			INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/help/ DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/help)
		ELSE()
			# Assume linux
			INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/help/ DESTINATION ${CMAKE_INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/help)
		ENDIF()
	ELSEIF(WIN32)
		ADD_CUSTOM_COMMAND(
			OUTPUT ${USERDOCS_INDEX}
			COMMENT "Generating user documentation.."
			COMMAND ${PERL_EXECUTABLE} ${PROJECT_SOURCE_DIR}/admin/gendoc.pl -v ${VERSION_RELEASE} ${CMAKE_CURRENT_BINARY_DIR}/help/en/ ${PROJECT_SOURCE_DIR}/data/doctemplates/ ${PROJECT_SOURCE_DIR}/src/
		)
		INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/help/ DESTINATION ${CMAKE_INSTALL_PREFIX}/help)
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
