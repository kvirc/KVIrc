# CMakeLists.txt for src/kvilib/

INCLUDE_DIRECTORIES(
	config
	core
	ext
	file
	irc
	net
	system
	${QT_INCLUDES}
)

SET(kvilib_MOC_HDRS
	config/kvi_buildinfo.h
	config/kvi_confignames.h
	config/kvi_debug.h
	config/kvi_defaults.h
	config/kvi_fileextensions.h
	config/kvi_settings.h
	config/kvi_sourcesdate.h
	config/kvi_version.h
	config/kvi_wincfg.h
	core/kvi_bswap.h
	core/kvi_error.h
	core/kvi_heapobject.h
	core/kvi_inttypes.h
	core/kvi_malloc.h
	core/kvi_memmove.h
	core/kvi_qcstring.h
	core/kvi_qstring.h
	core/kvi_strasm.h
	core/kvi_string.h
	core/kvi_stringarray.h
	ext/kvi_accel.h
	ext/kvi_cmdformatter.h
	ext/kvi_config.h
	ext/kvi_crypt.h
	ext/kvi_databuffer.h
	ext/kvi_dcophelper.h
	ext/kvi_doublebuffer.h
	ext/kvi_draganddrop.h
	ext/kvi_garbage.h
	ext/kvi_md5.h
	ext/kvi_mediatype.h
	ext/kvi_miscutils.h
	ext/kvi_msgtype.h
	ext/kvi_osinfo.h
	ext/kvi_pixmap.h
	ext/kvi_proxydb.h
	ext/kvi_regchan.h
	ext/kvi_regusersdb.h
	ext/kvi_sharedfiles.h
	ext/kvi_stringconversion.h
	ext/kvi_xlib.h
	file/kvi_file.h
	file/kvi_fileutils.h
	file/kvi_packagefile.h
	irc/kvi_avatar.h
	irc/kvi_avatarcache.h
	irc/kvi_ircmask.h
	irc/kvi_ircserver.h
	irc/kvi_ircserverdb.h
	irc/kvi_ircuserdb.h
	irc/kvi_mirccntrl.h
	irc/kvi_nickserv.h
	irc/kvi_useridentity.h
	net/kvi_dns.h
	net/kvi_http.h
	net/kvi_netutils.h
	net/kvi_socket.h
	net/kvi_sockettype.h
	net/kvi_ssl.h
	net/kvi_url.h
	system/kvi_byteorder.h
	system/kvi_env.h
	system/kvi_locale.h
	system/kvi_process.h
	system/kvi_stdarg.h
	system/kvi_thread.h
	system/kvi_time.h
	tal/kvi_tal_popupmenu.h
	tal/kvi_tal_application.h
	tal/kvi_tal_groupbox.h
	tal/kvi_tal_hbox.h
	tal/kvi_tal_iconview.h
	tal/kvi_tal_itemdelegates.h
	tal/kvi_tal_listbox.h
	tal/kvi_tal_listview.h
	tal/kvi_tal_listwidget.h
	tal/kvi_tal_menubar.h
	tal/kvi_tal_scrollview.h
	tal/kvi_tal_tabdialog.h
	tal/kvi_tal_toolbardocktype.h
	tal/kvi_tal_tooltip.h
	tal/kvi_tal_textedit.h
	tal/kvi_tal_treewidget.h
	tal/kvi_tal_vbox.h
	tal/kvi_tal_widgetstack.h
	tal/kvi_tal_windowstate.h
	tal/kvi_tal_wizard.h
)

IF(COMPILE_KDE_SUPPORT)
	LIST(APPEND kvilib_MOC_HDRS
		tal/kvi_tal_application_kde.h
		tal/kvi_tal_menubar_kde.h
		tal/kvi_tal_filedialog_kde.h
		tal/kvi_tal_mainwindow_kde.h
		tal/kvi_tal_toolbar_kde.h
	)
ELSE()
	LIST(APPEND kvilib_MOC_HDRS
		tal/kvi_tal_application_qt.h
		tal/kvi_tal_menubar_qt.h
		tal/kvi_tal_filedialog_qt.h
		tal/kvi_tal_mainwindow_qt.h
		tal/kvi_tal_toolbar_qt.h
	)
ENDIF()

IF(COMPILE_DBUS_SUPPORT)
	LIST(APPEND kvilib_MOC_SRCS
	ext/kvi_dbusadaptor.cpp
	)
	LIST(APPEND kvilib_MOC_HDRS
	ext/kvi_dbusadaptor.h
	)
ENDIF()

SET(kvilib_SRCS
	config/kvi_buildinfo.cpp
	core/kvi_error.cpp
	core/kvi_heapobject.cpp
	core/kvi_malloc.cpp
	core/kvi_memmove.cpp
	core/kvi_qstring.cpp
	core/kvi_string.cpp
	core/kvi_stringarray.cpp
	ext/kvi_cmdformatter.cpp
	ext/kvi_config.cpp
	ext/kvi_crypt.cpp
	ext/kvi_databuffer.cpp
	ext/kvi_dcophelper.cpp
	ext/kvi_doublebuffer.cpp
	ext/kvi_garbage.cpp
	ext/kvi_md5.cpp
	ext/kvi_mediatype.cpp
	ext/kvi_miscutils.cpp
	ext/kvi_msgtype.cpp
	ext/kvi_osinfo.cpp
	ext/kvi_pixmap.cpp
	ext/kvi_proxydb.cpp
	ext/kvi_regchan.cpp
	ext/kvi_regusersdb.cpp
	ext/kvi_sharedfiles.cpp
	ext/kvi_stringconversion.cpp
	file/kvi_file.cpp
	file/kvi_fileutils.cpp
	file/kvi_packagefile.cpp
	irc/kvi_avatar.cpp
	irc/kvi_avatarcache.cpp
	irc/kvi_ircmask.cpp
	irc/kvi_ircserver.cpp
	irc/kvi_ircserverdb.cpp
	irc/kvi_ircuserdb.cpp
	irc/kvi_mirccntrl.cpp
	irc/kvi_nickserv.cpp
	irc/kvi_useridentity.cpp
	net/kvi_dns.cpp
	net/kvi_http.cpp
	net/kvi_netutils.cpp
	net/kvi_socket.cpp
	net/kvi_ssl.cpp
	net/kvi_url.cpp
	system/kvi_env.cpp
	system/kvi_locale.cpp
	system/kvi_thread.cpp
	system/kvi_time.cpp
	tal/kvi_tal_application.cpp
	tal/kvi_tal_filedialog.cpp
	tal/kvi_tal_groupbox.cpp
	tal/kvi_tal_hbox.cpp
	tal/kvi_tal_iconview.cpp
	tal/kvi_tal_itemdelegates.cpp
	tal/kvi_tal_listbox.cpp
	tal/kvi_tal_listview.cpp
	tal/kvi_tal_listwidget.cpp
	tal/kvi_tal_mainwindow.cpp
	tal/kvi_tal_menubar.cpp
	tal/kvi_tal_popupmenu.cpp
	tal/kvi_tal_scrollview.cpp
	tal/kvi_tal_tabdialog.cpp
	tal/kvi_tal_textedit.cpp
	tal/kvi_tal_toolbar.cpp
	tal/kvi_tal_tooltip.cpp
	tal/kvi_tal_treewidget.cpp
	tal/kvi_tal_vbox.cpp
	tal/kvi_tal_widgetstack.cpp
	tal/kvi_tal_wizard.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvilib_MOC_SRCS ${kvilib_MOC_HDRS})

# Create library
IF(WANT_COEXISTENCE)
	SET(KVILIB_BINARYNAME kvilib${VERSION_MAJOR})
ELSE()
	SET(KVILIB_BINARYNAME kvilib)
ENDIF()

ADD_LIBRARY(${KVILIB_BINARYNAME} SHARED ${kvilib_SRCS} ${kvilib_MOC_SRCS})
TARGET_LINK_LIBRARIES(${KVILIB_BINARYNAME} ${LIBS})

# Installation directives
SET_TARGET_PROPERTIES(${KVILIB_BINARYNAME} PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
IF(UNIX)
	IF(APPLE)
		SET_TARGET_PROPERTIES(${KVILIB_BINARYNAME} PROPERTIES INSTALL_NAME_DIR "@executable_path/../Frameworks")
		INSTALL(TARGETS ${KVILIB_BINARYNAME} LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Frameworks)
	ELSE()
		# Assume linux
		IF(NOT DEFINED LIB_INSTALL_PREFIX)
			INSTALL(TARGETS ${KVILIB_BINARYNAME} LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
		ELSE()
			INSTALL(TARGETS ${KVILIB_BINARYNAME} LIBRARY DESTINATION ${LIB_INSTALL_PREFIX})
		ENDIF()
	ENDIF()
ELSEIF(WIN32)
	# INSTALL(TARGETS ${KVILIB_BINARYNAME} LIBRARY DESTINATION ???)
ENDIF()
