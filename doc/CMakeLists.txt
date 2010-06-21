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
	addon.howto.txt
	firefox.hack.txt
	hackers.guide.txt
	scripting.faq.txt
	themes.howto.txt
	translations.howto.txt
	ui-style.tips.txt
)

# User documentation
IF(PERL_FOUND)

	# The userdocs target is in the normal build process and rebuilds the documentation
	# only when the revision number changes. This is useful for svn users and package users.

	# The docs target is standalone and explicitly builds the documentation (useful for developers)

	IF(DEFINED CMAKE_KVIRC_BUILD_REVISION)
		SET(USERDOCS_VERSION "${VERSION_RELEASE}.${CMAKE_KVIRC_BUILD_REVISION}")
	ELSE()
		SET(USERDOCS_VERSION "${VERSION_RELEASE}")
	ENDIF()

	SET(USERDOCS_OUTPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/help/en/documentation.${USERDOCS_VERSION}.tag")

	ADD_CUSTOM_TARGET(userdocs ALL)
	ADD_CUSTOM_TARGET(userdocs-en DEPENDS ${USERDOCS_OUTPUT_FILE})
	ADD_DEPENDENCIES(userdocs userdocs-en)

	IF(UNIX)
		ADD_CUSTOM_COMMAND(
			OUTPUT ${USERDOCS_OUTPUT_FILE}
			COMMENT "Generating user documentation..."
			COMMAND ${PERL_EXECUTABLE} ${PROJECT_SOURCE_DIR}/admin/gendoc.pl -v ${USERDOCS_VERSION} '${CMAKE_CURRENT_BINARY_DIR}/help/en/' ${PROJECT_SOURCE_DIR}/data/doctemplates/ ${PROJECT_SOURCE_DIR}/src/
		)
		IF(APPLE)
			INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/help/ DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/help)
		ELSE()
			# Assume linux
			INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/help/ DESTINATION ${CMAKE_INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/help)
		ENDIF()
		ADD_CUSTOM_TARGET(
			docs
			COMMENT "Generating user documentation..."
			COMMAND ${PERL_EXECUTABLE} ${PROJECT_SOURCE_DIR}/admin/gendoc.pl -v ${USERDOCS_VERSION} '${CMAKE_CURRENT_BINARY_DIR}/help/en/' ${PROJECT_SOURCE_DIR}/data/doctemplates/ ${PROJECT_SOURCE_DIR}/src/
		)
	ELSEIF(WIN32)
		ADD_CUSTOM_COMMAND(
			OUTPUT ${USERDOCS_OUTPUT_FILE}
			COMMENT "Generating user documentation..."
			COMMAND ${PERL_EXECUTABLE} ${PROJECT_SOURCE_DIR}/admin/gendoc.pl -v ${USERDOCS_VERSION} ${CMAKE_CURRENT_BINARY_DIR}/help/en/ ${PROJECT_SOURCE_DIR}/data/doctemplates/ ${PROJECT_SOURCE_DIR}/src/
		)
		INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/help/ DESTINATION ${CMAKE_INSTALL_PREFIX}/help)	
		ADD_CUSTOM_TARGET(
			docs
			COMMENT "Generating user documentation..."
			COMMAND ${PERL_EXECUTABLE} ${PROJECT_SOURCE_DIR}/admin/gendoc.pl -v ${USERDOCS_VERSION} ${CMAKE_CURRENT_BINARY_DIR}/help/en/ ${PROJECT_SOURCE_DIR}/data/doctemplates/ ${PROJECT_SOURCE_DIR}/src/
		)
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
