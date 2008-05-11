# CMakeLists for doc/scriptexamples/
SUBDIRS(kvisounds mp3share playing tutorial)

SET(scriptexamples
	README
	antiidle.kvs class1.kvs exprtest.kvs
	#
	label1.kvs mail.kvs minesweeper.kvs
	mp3_kdemediaplayers.kvs mp3_kdemediaplayers.txt
	mp3info.kvs
	#
	newkvstest.kvs objectsdumptree.kvs
	#
	popup1.kvs popup2.kvs popup3.kvs popup4.kvs relay.kvs
	runmenu.kvs signal1.kvs simplehttp.kvs socket1.kvs
	srfs.kvs switchstress.kvs timer1.kvs
	#
	widget1.kvs widget2.kvs
)

# Install target
IF(UNIX)
	IF(APPLE)
		INSTALL(FILES ${scriptexamples} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/doc/scriptexamples)
	ELSE()
		# Assume linux
		INSTALL(FILES ${scriptexamples} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/doc/scriptexamples)
	ENDIF()
ELSEIF(WIN32)
	#INSTALL(FILES ${scriptexamples} DESTINATION ${CMAKE_INSTALL_PREFIX}/ ??? /${VERSION_BRANCH}/doc/scriptexamples)
ENDIF()
