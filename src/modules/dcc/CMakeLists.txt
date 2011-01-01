# CMakeLists for src/modules/dcc

SET(kvidcc_MOC_HDRS
	DccBroker.h
	DccChatWindow.h
	DccDialog.h
	DccMarshal.h
	DccFileTransfer.h
	DccVoiceWindow.h
	DccWindow.h
)

IF(NOT DEFINED COMPILE_DISABLE_DCC_VIDEO) 
        LIST(APPEND kvidcc_MOC_HDRS 
                DccVideoWindow.h 
        ) 
ENDIF() 

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

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvidcc_MOC_SRCS ${kvidcc_MOC_HDRS})
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

