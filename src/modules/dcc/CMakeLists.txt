# CMakeLists for src/modules/dcc

set(kvidcc_SRCS
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

if(NOT COMPILE_DISABLE_DCC_VIDEO)
        list(APPEND kvidcc_SRCS
                DccVideoWindow.cpp
        )
endif()

set(files
	kvi_dccfiletransfericons.png
)

set(kvi_module_name kvidcc)
include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)

if(UNIX)
	if(APPLE)
		install(FILES ${files} DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/Resources/pics/)
	else()
		# Assume linux
		install(FILES ${files} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/pics/)
	endif()
elseif(WIN32)
	install(FILES ${files} DESTINATION ${CMAKE_INSTALL_PREFIX}/pics/)
endif()
