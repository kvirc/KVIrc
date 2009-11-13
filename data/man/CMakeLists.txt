# CMakeLists for data/man

FILE(GLOB MANS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.1")

IF(UNIX)
	IF(APPLE)
		# does macosx use manpages?
	ELSE()
		# Assume linux
		FOREACH(MAN ${MANS})
			# Grep the language code
			SET(LANGCODE ${MAN})
			STRING(REPLACE "kvirc." "" LANGCODE ${LANGCODE})
			STRING(REPLACE ".1" "" LANGCODE ${LANGCODE})

			# If the man is in English, it uses the root directory
			STRING(COMPARE EQUAL ${LANGCODE} "en" STREQUAL)
			IF(${STREQUAL})
				INSTALL(FILES ${MAN} DESTINATION ${CMAKE_INSTALL_PREFIX}/${MANDIR_PREFIX}/man1/ RENAME "kvirc.1")
			ELSE()
				INSTALL(FILES ${MAN} DESTINATION ${CMAKE_INSTALL_PREFIX}/${MANDIR_PREFIX}/${LANGCODE}/man1/ RENAME "kvirc.1")
			ENDIF()
		ENDFOREACH()
	ENDIF()
ENDIF()
