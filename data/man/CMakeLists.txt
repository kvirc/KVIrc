# CMakeLists for data/man

file(GLOB MANS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.1")

if(UNIX)
	if(APPLE)
		# does macosx use manpages?
	else()
		# Assume linux
		foreach(MAN ${MANS})
			# Grep the language code
			set(LANGCODE ${MAN})
			string(REPLACE "kvirc." "" LANGCODE ${LANGCODE})
			string(REPLACE ".1" "" LANGCODE ${LANGCODE})

			# If the man is in English, it uses the root directory
			string(COMPARE EQUAL ${LANGCODE} "en" STREQUAL)
			if(${STREQUAL})
				install(FILES ${MAN} DESTINATION ${CMAKE_INSTALL_PREFIX}/${MANDIR_PREFIX}/man1/ RENAME "kvirc.1")
			else()
				install(FILES ${MAN} DESTINATION ${CMAKE_INSTALL_PREFIX}/${MANDIR_PREFIX}/${LANGCODE}/man1/ RENAME "kvirc.1")
			endif()
		endforeach()
	endif()
endif()
