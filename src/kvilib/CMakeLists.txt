# CMakeLists.txt for src/kvilib/

include_directories(
	config
	core
	ext
	file
	irc
	locale
	net
	system
)

# Please note that the sources have alphabetic order here

set(kvilib_SRCS
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
	net/KviNetworkAccessManager.cpp
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

if(NOT DEFINED COMPILE_DISABLE_AVDEVICE)
	list(APPEND kvilib_SRCS
		ext/avdevice/videodevice.cpp 
		ext/avdevice/videoinput.cpp 
		ext/avdevice/videodevicepool.cpp 
		ext/avdevice/bayer.cpp
		ext/avdevice/sonix_compress.cpp
	)
endif()

if(NOT DEFINED COMPILE_DISABLE_OGG_THEORA)
	list(APPEND kvilib_SRCS
		ext/KviOggTheoraDecoder.cpp
		ext/KviOggTheoraEncoder.cpp
	)
endif()

# Create library
if(WANT_COEXISTENCE)
	set(KVILIB_BINARYNAME kvilib${VERSION_MAJOR})
else()
	set(KVILIB_BINARYNAME kvilib)
endif()

#we need this defined when mingw will compile moc files
if(WIN32)
	add_definitions(-D__KVILIB__)
endif()

if(MSVC)
	list(APPEND LIBS wsock32.lib ws2_32.lib)
endif()

add_library(${KVILIB_BINARYNAME} SHARED ${kvilib_SRCS} ${kvilib_MOC_SRCS})
target_link_libraries(${KVILIB_BINARYNAME} ${LIBS})

if(Qt5Widgets_FOUND)
	qt5_use_modules(${KVILIB_BINARYNAME} ${qt5_kvirc_modules})
endif()

set_target_properties(${KVILIB_BINARYNAME} PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR} LINK_FLAGS "${ADDITIONAL_LINK_FLAGS}")

if(WANT_STRIP)
	get_target_property(KVILIB_LOCATION ${KVILIB_BINARYNAME} LOCATION)
	install(CODE "exec_program(${STRIP_EXECUTABLE} ARGS -s \"${KVILIB_LOCATION}\")")
endif()

# Installation directives
if(UNIX)
	install(TARGETS ${KVILIB_BINARYNAME} LIBRARY DESTINATION "${KVIRC_LIB_PATH}")
elseif(WIN32)
	# For DLL platforms the DLL part of a shared library is treated as a RUNTIME target and the corresponding import library is treated as an ARCHIVE target
	install(TARGETS ${KVILIB_BINARYNAME} RUNTIME DESTINATION "${KVIRC_LIB_PATH}")
endif()
