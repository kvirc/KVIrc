# CMakeLists for doc/

set(license
	ABOUT-LICENSE
	LICENSE-OPENSSL
	../COPYING
)
if(WIN32)
	list(APPEND license LICENSE-AMIP)
endif()

# User documentation
if(PERL_FOUND)

	# The userdocs target is in the normal build process and rebuilds the documentation
	# only when the revision number changes. This is useful for Git users and package users.

	# The docs target is standalone and explicitly builds the documentation (useful for developers)

	if(DEFINED CMAKE_KVIRC_BUILD_REVISION)
		set(USERDOCS_VERSION "${VERSION_RELEASE}.${CMAKE_KVIRC_BUILD_REVISION}")
	else()
		set(USERDOCS_VERSION "${VERSION_RELEASE}")
	endif()

	set(USERDOCS_OUTPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/help/en/documentation.${USERDOCS_VERSION}.tag")

	add_custom_target(userdocs ALL)
	add_custom_target(userdocs-en DEPENDS ${USERDOCS_OUTPUT_FILE})
	add_dependencies(userdocs userdocs-en)

	if(UNIX)
		add_custom_command(
			OUTPUT ${USERDOCS_OUTPUT_FILE}
			COMMENT "Generating user documentation..."
			COMMAND ${PERL_EXECUTABLE} ${PROJECT_SOURCE_DIR}/admin/gendoc.pl --css ${PROJECT_SOURCE_DIR}/admin/doc.css -v ${USERDOCS_VERSION} '${CMAKE_CURRENT_BINARY_DIR}/help/en/' ${PROJECT_SOURCE_DIR}/data/doctemplates/ ${PROJECT_SOURCE_DIR}/src/
		)
		if(APPLE)
			install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/help/ DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/help)
		else()
			# Assume linux
			install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/help/ DESTINATION ${CMAKE_INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/help)
		endif()
		add_custom_target(
			docs
			COMMENT "Generating user documentation..."
			COMMAND ${PERL_EXECUTABLE} ${PROJECT_SOURCE_DIR}/admin/gendoc.pl --css ${PROJECT_SOURCE_DIR}/admin/doc.css -v ${USERDOCS_VERSION} '${CMAKE_CURRENT_BINARY_DIR}/help/en/' ${PROJECT_SOURCE_DIR}/data/doctemplates/ ${PROJECT_SOURCE_DIR}/src/
		)
	elseif(WIN32)
		add_custom_command(
			OUTPUT ${USERDOCS_OUTPUT_FILE}
			COMMENT "Generating user documentation..."
			COMMAND ${PERL_EXECUTABLE} ${PROJECT_SOURCE_DIR}/admin/gendoc.pl --css ${PROJECT_SOURCE_DIR}/admin/doc.css -v ${USERDOCS_VERSION} ${CMAKE_CURRENT_BINARY_DIR}/help/en/ ${PROJECT_SOURCE_DIR}/data/doctemplates/ ${PROJECT_SOURCE_DIR}/src/
		)
		install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/help/ DESTINATION ${CMAKE_INSTALL_PREFIX}/help)
		add_custom_target(
			docs
			COMMENT "Generating user documentation..."
			COMMAND ${PERL_EXECUTABLE} ${PROJECT_SOURCE_DIR}/admin/gendoc.pl --css ${PROJECT_SOURCE_DIR}/admin/doc.css -v ${USERDOCS_VERSION} ${CMAKE_CURRENT_BINARY_DIR}/help/en/ ${PROJECT_SOURCE_DIR}/data/doctemplates/ ${PROJECT_SOURCE_DIR}/src/
		)
	endif()

endif()

# Install target
if(UNIX)
	if(APPLE)
		install(FILES ${license} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/license)
	else()
		# Assume linux
		install(FILES ${license} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/license/)
	endif()
elseif(WIN32)
	install(FILES ${license} DESTINATION ${CMAKE_INSTALL_PREFIX}/license/)
endif()
