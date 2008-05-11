# CMakeLists for doc/scriptexamples/tutorial/

SET(tutorial
	minesweeper1.kvs
	minesweeper2.kvs
	minesweeper3.kvs
	minesweeper4.kvs
	minesweeper5.kvs
	minesweeper6.kvs
	minesweeper7.kvs
)

# Install target
IF(UNIX)
	IF(APPLE)
		INSTALL(FILES ${tutorial} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/doc/scriptexamples/tutorial)
	ELSE()
		# Assume linux
		INSTALL(FILES ${tutorial} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/doc/scriptexamples/tutorial)
	ENDIF()
ELSEIF(WIN32)
	#INSTALL(FILES ${tutorial} DESTINATION ${CMAKE_INSTALL_PREFIX}/ ??? /${VERSION_BRANCH}/doc/scriptexamples/tutorial)
ENDIF()
