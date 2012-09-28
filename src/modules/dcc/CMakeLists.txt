# CMakeLists for src/modules/dcc

SET(kvidcc_SRCS
	DccVoiceAdpcmCodec.cpp
	DccBroker.cpp
	DccCanvasWindow.cpp
	canvaswidget.cpp
	DccChatWindow.cpp
	DccVoiceCodec.cpp
	DccDescriptor.cpp
	DccDialog.cpp
	DccVoiceGsmCodec.cpp
	libkvidcc.cpp
	DccMarshal.cpp
	requests.cpp
	DccFileTransfer.cpp
	DccThread.cpp
	DccUtils.cpp
	DccVoiceWindow.cpp
	DccWindow.cpp
)

IF(NOT DEFINED COMPILE_DISABLE_DCC_VIDEO) 
        LIST(APPEND kvidcc_SRCS 
                DccVideoWindow.cpp 
        ) 
ENDIF()

SET(files
	kvi_dccfiletransfericons.png
)

IF(WIN32)
	LIST(APPEND LIBS -lws2_32)
ENDIF()

SET(kvi_module_name kvidcc)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)

IF(UNIX)
	IF(APPLE)
		INSTALL(FILES ${files} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/pics/)
	ELSE()
		# Assume linux
		INSTALL(FILES ${files} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/pics/)
	ENDIF()
ELSEIF(WIN32)
	INSTALL(FILES ${files} DESTINATION ${CMAKE_INSTALL_PREFIX}/pics/)
ENDIF()

