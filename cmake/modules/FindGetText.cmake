FIND_PROGRAM(GETTEXT_MSGMERGE_EXECUTABLE msgmerge)
FIND_PROGRAM(GETTEXT_MSGFMT_EXECUTABLE msgfmt)

MACRO(GETTEXT_CREATE_TRANSLATIONS _potFile _firstPoFile)
	SET(_gmoFiles)
	GET_FILENAME_COMPONENT(_potBasename ${_potFile} NAME_WE)
	GET_FILENAME_COMPONENT(_absPotFile ${_potFile} ABSOLUTE)

	SET(_addToAll)
	IF(${_firstPoFile} STREQUAL "ALL")
		SET(_addToAll "ALL")
		SET(_firstPoFile)
	ENDIF(${_firstPoFile} STREQUAL "ALL")

	FOREACH (_currentPoFile ${ARGN})
		GET_FILENAME_COMPONENT(_absFile ${_currentPoFile} ABSOLUTE)
		GET_FILENAME_COMPONENT(_abs_PATH ${_absFile} PATH)
		GET_FILENAME_COMPONENT(_lang ${_absFile} NAME_WE)
		SET(_gmoFile ${CMAKE_CURRENT_BINARY_DIR}/${_lang}.gmo)

		ADD_CUSTOM_COMMAND(
			OUTPUT ${_gmoFile}
			# do not update *.po during the standard build
			#COMMAND ${GETTEXT_MSGMERGE_EXECUTABLE} --quiet --update --backup=none -s ${_absFile} ${_absPotFile}
			COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} -o ${_gmoFile} ${_absFile}
			DEPENDS ${_absPotFile} ${_absFile}
		)
		IF(UNIX)
			IF(APPLE)
				INSTALL(FILES ${_gmoFile} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/locale RENAME ${_lang}.mo)
			ELSE()
				# Assume linux
				INSTALL(FILES ${_gmoFile} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/locale RENAME ${_lang}.mo)
			ENDIF()
		ELSEIF(WIN32)
			# FIXME: Use proper path
			#INSTALL(FILES ${_gmoFile} DESTINATION share/locale/${_lang}/LC_MESSAGES RENAME ${_potBasename}.mo)
		ENDIF()
		SET(_gmoFiles ${_gmoFiles} ${_gmoFile})
	ENDFOREACH (_currentPoFile )

	ADD_CUSTOM_TARGET(translations ${_addToAll} DEPENDS ${_gmoFiles})
ENDMACRO(GETTEXT_CREATE_TRANSLATIONS )

IF (GETTEXT_MSGMERGE_EXECUTABLE AND GETTEXT_MSGFMT_EXECUTABLE )
	SET(GETTEXT_FOUND TRUE)
ELSE (GETTEXT_MSGMERGE_EXECUTABLE AND GETTEXT_MSGFMT_EXECUTABLE )
	SET(GETTEXT_FOUND FALSE)
	IF (GetText_REQUIRED)
		MESSAGE(FATAL_ERROR "GetText not found")
	ENDIF (GetText_REQUIRED)
ENDIF (GETTEXT_MSGMERGE_EXECUTABLE AND GETTEXT_MSGFMT_EXECUTABLE )
