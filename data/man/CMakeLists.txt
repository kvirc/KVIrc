# CMakeLists for data/man

FILE(GLOB MANS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.1")

IF(UNIX)
	IF(APPLE)
		# does macosx use manpages?
	ELSE()
		# Assume linux
		FOREACH(MAN ${MANS})
			# Grep the language code
			# FIXME: this doesn't work with codes like "pt_BR"
			STRING(SUBSTRING ${MAN} 6 2 LANGCODE)

			# If the man is in English, it uses the root directory
			STRING(COMPARE EQUAL ${LANGCODE} "en" STREQUAL)
			IF(${STREQUAL})
				STRING(REGEX REPLACE ".en" "" MAN ${MAN})
				INSTALL(FILES ${MAN} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/man/man1/)
			ELSE()
				INSTALL(FILES ${MAN} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/man/${LANGCODE}/man1/)
			ENDIF()
		ENDFOREACH()
	ENDIF()
ENDIF()
