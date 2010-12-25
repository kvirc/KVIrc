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

# Please note that the sources have alphabetic order here

SET(kvilib_MOC_HDRS
	ext/KviAnimatedPixmap.h
	ext/KviAnimatedPixmapCache.h
	ext/KviCryptEngine.h
	ext/KviRegisteredUserDataBase.h
	ext/KviSharedFilesManager.h
	irc/KviIrcUserDataBase.h
	net/kvi_dns.h
	net/kvi_http.h
	system/KviLocale.h
	system/KviThread.h
	tal/kvi_tal_application.h
	tal/kvi_tal_filedialog.h
	tal/kvi_tal_groupbox.h
	tal/kvi_tal_hbox.h
	tal/kvi_tal_listwidget.h
	tal/kvi_tal_mainwindow.h
	tal/kvi_tal_menubar.h
	tal/kvi_tal_popupmenu.h
	tal/kvi_tal_tabdialog.h
	tal/kvi_tal_tablewidget.h
	tal/kvi_tal_toolbar.h
	tal/kvi_tal_tooltip.h
	tal/kvi_tal_vbox.h
	tal/kvi_tal_wizard.h
)

IF(COMPILE_DBUS_SUPPORT)
	LIST(APPEND kvilib_MOC_HDRS
		ext/KviDbusAdaptor.h
	)
ENDIF()

IF(NOT DEFINED COMPILE_DISABLE_AVDEVICE)
	LIST(APPEND kvilib_MOC_HDRS
		ext/avdevice/videodevicepool.h
	)
ENDIF()

# Please note that the sources have alphabetic order here

SET(kvilib_SRCS
	config/kvi_buildinfo.cpp
	core/kvi_error.cpp
	core/KviHeapObject.cpp
	core/kvi_hstrEqualCIN.cpp
	core/KviMemory.cpp
	core/kvi_memmove.cpp
	core/KviQString.cpp
	core/KviCString.cpp
	ext/KviCommandFormatter.cpp
	ext/KviConfigurationFile.cpp
	ext/KviCryptEngine.cpp
	ext/KviDataBuffer.cpp
	ext/KviDbusAdaptor.cpp
	ext/KviDebugContext.cpp
	ext/KviMediaType.cpp
	ext/KviMiscUtils.cpp
	ext/KviMessageTypeSettings.cpp
	ext/KviOsInfo.cpp
	ext/KviPixmap.cpp
	ext/KviAnimatedPixmap.cpp
	ext/KviAnimatedPixmapCache.cpp
	ext/KviNickColors.cpp
	ext/KviProxyDataBase.cpp
	ext/KviRegisteredChannel.cpp
	ext/KviRegisteredUserDataBase.cpp
	ext/KviSharedFilesManager.cpp
	ext/KviStringConversion.cpp
	file/KviFile.cpp
	file/KviFileUtils.cpp
	file/KviPackageFile.cpp
	irc/KviAvatar.cpp
	irc/KviAvatarCache.cpp
	irc/KviIdentityProfile.cpp
	irc/KviIrcMask.cpp
	irc/KviIrcServer.cpp
	irc/KviIrcServerDataBase.cpp
	irc/KviIrcUserDataBase.cpp
	irc/KviMircCntrl.cpp
	irc/KviNetwork.cpp
	irc/KviNickServRuleSet.cpp
	irc/KviUserIdentity.cpp
	net/kvi_dns.cpp
	net/kvi_http.cpp
	net/kvi_netutils.cpp
	net/kvi_sasl.cpp
	net/kvi_socket.cpp
	net/kvi_ssl.cpp
	net/kvi_url.cpp
	system/KviEnvironment.cpp
	system/KviLocale.cpp
	system/KviThread.cpp
	system/KviTimeUtils.cpp
	tal/kvi_tal_application.cpp
	tal/kvi_tal_filedialog.cpp
	tal/kvi_tal_groupbox.cpp
	tal/kvi_tal_hbox.cpp
	tal/kvi_tal_itemdelegates.cpp
	tal/kvi_tal_listwidget.cpp
	tal/kvi_tal_mainwindow.cpp
	tal/kvi_tal_menubar.cpp
	tal/kvi_tal_popupmenu.cpp
	tal/kvi_tal_splitter.cpp
	tal/kvi_tal_tabdialog.cpp
	tal/kvi_tal_tablewidget.cpp
	tal/kvi_tal_toolbar.cpp
	tal/kvi_tal_tooltip.cpp
	tal/kvi_tal_vbox.cpp
	tal/kvi_tal_wizard.cpp
)

IF(NOT DEFINED COMPILE_DISABLE_AVDEVICE)
	LIST(APPEND kvilib_SRCS
		ext/avdevice/videodevice.cpp 
		ext/avdevice/videoinput.cpp 
		ext/avdevice/videodevicepool.cpp 
		ext/avdevice/bayer.cpp
		ext/avdevice/sonix_compress.cpp
	)
ENDIF()

IF(NOT DEFINED COMPILE_DISABLE_OGG_THEORA)
	LIST(APPEND kvilib_SRCS
		ext/KviTheoraDecoder.cpp
	)
ENDIF()

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvilib_MOC_SRCS ${kvilib_MOC_HDRS})

# Create library
IF(WANT_COEXISTENCE)
	SET(KVILIB_BINARYNAME kvilib${VERSION_MAJOR})
ELSE()
	SET(KVILIB_BINARYNAME kvilib)
ENDIF()

#we need this defined when mingw will compile moc files
IF(WIN32)
        ADD_DEFINITIONS(-D__KVILIB__)
ENDIF()

ADD_LIBRARY(${KVILIB_BINARYNAME} SHARED ${kvilib_SRCS} ${kvilib_MOC_SRCS})
TARGET_LINK_LIBRARIES(${KVILIB_BINARYNAME} ${LIBS})
SET_TARGET_PROPERTIES(${KVILIB_BINARYNAME} PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR} LINK_FLAGS "${ADDITIONAL_LINK_FLAGS}")

IF(WANT_STRIP)
	GET_TARGET_PROPERTY(KVILIB_LOCATION ${KVILIB_BINARYNAME} LOCATION)
	INSTALL(CODE "EXEC_PROGRAM(${STRIP_EXECUTABLE} ARGS -s \"${KVILIB_LOCATION}\")")
ENDIF()

# Installation directives
IF(UNIX)
	IF(APPLE)
		SET_TARGET_PROPERTIES(${KVILIB_BINARYNAME} PROPERTIES INSTALL_NAME_DIR "@executable_path/../Frameworks")
		INSTALL(TARGETS ${KVILIB_BINARYNAME} LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Frameworks)
	ELSE()
		# Assume linux
		INSTALL(TARGETS ${KVILIB_BINARYNAME} LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}")
	ENDIF()
ELSEIF(WIN32)
	# For DLL platforms the DLL part of a shared library is treated as a RUNTIME target and the corresponding import library is treated as an ARCHIVE target
	INSTALL(TARGETS ${KVILIB_BINARYNAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
ENDIF()
