# CMakeLists.txt for src/kvilib/

INCLUDE_DIRECTORIES(
	config
	core
	ext
	file
	irc
	locale
	net
	system
	${QT_INCLUDES}
)

# Please note that the sources have alphabetic order here

SET(
	kvilib_MOC_HDRS
	ext/KviAnimatedPixmap.h
	ext/KviAnimatedPixmapCache.h
	ext/KviCryptEngine.h
	ext/KviRegisteredUserDataBase.h
	ext/KviSharedFilesManager.h
	irc/KviIrcUserDataBase.h
	locale/KviTranslator.h
	net/KviDnsResolver.h
	net/KviHttpRequest.h
	system/KviThread.h
	tal/KviTalApplication.h
	tal/KviTalFileDialog.h
	tal/KviTalGroupBox.h
	tal/KviTalHBox.h
	tal/KviTalListWidget.h
	tal/KviTalMainWindow.h
	tal/KviTalMenuBar.h
	tal/KviTalTabDialog.h
	tal/KviTalTableWidget.h
	tal/KviTalToolTip.h
	tal/KviTalVBox.h
	tal/KviTalWizard.h
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
	config/KviBuildInfo.cpp
	core/KviError.cpp
	core/KviHeapObject.cpp
	core/KviMemory.cpp
	core/KviQString.cpp
	core/KviCString.cpp
	core/KviShortcut.cpp
	ext/KviCommandFormatter.cpp
	ext/KviConfigurationFile.cpp
	ext/KviCryptEngine.cpp
	ext/KviCryptEngineManager.cpp
	ext/KviDataBuffer.cpp
	ext/KviDbusAdaptor.cpp
	ext/KviDebugContext.cpp
	ext/KviMediaManager.cpp
	ext/KviMiscUtils.cpp
	ext/KviMessageTypeSettings.cpp
	ext/KviOsInfo.cpp
	ext/KviPixmap.cpp
	ext/KviPixmapUtils.cpp
	ext/KviAnimatedPixmap.cpp
	ext/KviAnimatedPixmapCache.cpp
	ext/KviNickColors.cpp
	ext/KviProxy.cpp
	ext/KviProxyDataBase.cpp
	ext/KviRegisteredChannel.cpp
	ext/KviRegisteredChannelDataBase.cpp
	ext/KviRegisteredUser.cpp
	ext/KviRegisteredUserDataBase.cpp
	ext/KviRegisteredUserGroup.cpp
	ext/KviRegisteredUserMask.cpp
	ext/KviSharedFile.cpp
	ext/KviSharedFilesManager.cpp
	ext/KviStringConversion.cpp
	file/KviFile.cpp
	file/KviFileUtils.cpp
	file/KviPackageIOEngine.cpp
	file/KviPackageReader.cpp
	file/KviPackageWriter.cpp
	irc/KviAvatar.cpp
	irc/KviAvatarCache.cpp
	irc/KviControlCodes.cpp
	irc/KviIdentityProfile.cpp
	irc/KviIdentityProfileSet.cpp
	irc/KviIrcMask.cpp
	irc/KviIrcNetwork.cpp
	irc/KviIrcServer.cpp
	irc/KviIrcServerDataBase.cpp
	irc/KviIrcServerReconnectInfo.cpp
	irc/KviIrcUserDataBase.cpp
	irc/KviIrcUserEntry.cpp
	irc/KviNickServRule.cpp
	irc/KviNickServRuleSet.cpp
	irc/KviUserIdentity.cpp
	irc/KviUserIdentityManager.cpp
	locale/KviLocale.cpp
	locale/KviMessageCatalogue.cpp
	locale/KviTranslationEntry.cpp
	locale/KviTranslator.cpp
	net/KviDnsResolver.cpp
	net/KviHttpRequest.cpp
	net/KviNetUtils.cpp
	net/KviSASL.cpp
	net/kvi_socket.cpp
	net/KviSSL.cpp
	net/KviUrl.cpp
	system/KviEnvironment.cpp
	system/KviThread.cpp
	system/KviTimeUtils.cpp
	tal/KviTalApplication.cpp
	tal/KviTalFileDialog.cpp
	tal/KviTalGroupBox.cpp
	tal/KviTalHBox.cpp
	tal/KviTalIconAndRichTextItemDelegate.cpp
	tal/KviTalListWidget.cpp
	tal/KviTalMainWindow.cpp
	tal/KviTalMenuBar.cpp
	tal/KviTalSplitter.cpp
	tal/KviTalTabDialog.cpp
	tal/KviTalTableWidget.cpp
	tal/KviTalToolTip.cpp
	tal/KviTalVBox.cpp
	tal/KviTalWizard.cpp
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
		ext/KviOggTheoraDecoder.cpp
		ext/KviOggTheoraEncoder.cpp
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

IF(MSVC)
	LIST(APPEND LIBS wsock32.lib ws2_32.lib)
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
	INSTALL(TARGETS ${KVILIB_BINARYNAME} LIBRARY DESTINATION "${KVIRC_LIB_PATH}")
ELSEIF(WIN32)
	# For DLL platforms the DLL part of a shared library is treated as a RUNTIME target and the corresponding import library is treated as an ARCHIVE target
	INSTALL(TARGETS ${KVILIB_BINARYNAME} RUNTIME DESTINATION "${KVIRC_LIB_PATH}")
ENDIF()
