# CMakeLists for doc/

SET(license COPYING)
SET(howto
	README
	FAQ
	INSTALL-MacOS.txt
	firefox.hack.txt
	hackers.guide.txt
	scripting.faq.txt
	themes.howto.txt
	translations.howto.txt
	ui-style.tips.txt
	win32.compiling.1.2.txt
)
SET(scriptexamples
	README
	antiidle.kvs class1.kvs exprtest.kvs
	#
	kvisounds/
	kvisounds/kvisounds.kvs
	kvisounds/waves/
	kvisounds/waves/bip.au
	kvisounds/waves/cuac.au
	kvisounds/waves/deop.au
	kvisounds/waves/join.au
	kvisounds/waves/kick.au
	kvisounds/waves/login.au
	kvisounds/waves/op.au
	kvisounds/waves/ring.au
	kvisounds/waves/send.au
	kvisounds/waves/transfer.au
	#
	label1.kvs mail.kvs minesweeper.kvs
	mp3_kdemediaplayers.kvs mp3_kdemediaplayers.txt
	mp3info.kvs
	#
	mp3share/
	mp3share/mp3share.kvs
	mp3share/mp3share_icon.png
	#
	newkvstest.kvs obdumptree.kvs
	#
	playing/
	playing/playing.kvs
	playing/playing_icon.png
	#
	popup1.kvs popup2.kvs popup3.kvs popup4.kvs relay.kvs
	runmenu.kvs signal1.kvs simplehttp.kvs socket1.kvs
	srfs.kvs switchstress.kvs timer1.kvs
	#
	tutorial/
	tutorial/minesweeper1.kvs
	tutorial/minesweeper2.kvs
	tutorial/minesweeper3.kvs
	tutorial/minesweeper4.kvs
	tutorial/minesweeper5.kvs
	tutorial/minesweeper6.kvs
	tutorial/minesweeper7.kvs
	#
	widget1.kvs widget2.kvs
)

# Install target
IF(UNIX)
	IF(APPLE)
		INSTALL(FILES ${license} DESTINATION ${INSTALL_PREFIX}/Contents/Resources/license)
		INSTALL(FILES ${howto} DESTINATION ${INSTALL_PREFIX}/Contents/Resources/doc)
		INSTALL(FILES ${scriptexamples} DESTINATION ${INSTALL_PREFIX}/Contents/Resources/doc/scriptexamples)
	ELSE()
		# Assume linux
		INSTALL(FILES ${license} DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/license/)
		INSTALL(FILES ${howto} DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/doc/)
		INSTALL(FILES ${scriptexamples} DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/doc/scriptexamples)
	ENDIF()
ELSEIF(WIN32)
	#INSTALL(FILES ${license} DESTINATION ${INSTALL_PREFIX}/ ??? /${VERSION_BRANCH}/license/)
	#INSTALL(FILES ${howto} DESTINATION ${INSTALL_PREFIX}/ ??? /${VERSION_BRANCH}/doc/)
	#INSTALL(FILES ${scriptexamples} DESTINATION ${INSTALL_PREFIX}/ ??? /${VERSION_BRANCH}/doc/scriptexamples)
ENDIF()
