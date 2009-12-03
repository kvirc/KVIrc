# CMakeLists for src/modules/dcc

SET(kvidcc_MOC_HDRS
	broker.h
	chat.h
	dialogs.h
	marshal.h
	send.h
	voice.h
	window.h
)

IF(NOT DEFINED COMPILE_DISABLE_DCC_VIDEO) 
        LIST(APPEND kvidcc_MOC_HDRS 
                video.h 
        ) 
ENDIF() 

SET(kvidcc_SRCS
	adpcmcodec.cpp
	broker.cpp
	canvas.cpp
	canvaswidget.cpp
	chat.cpp
	codec.cpp
	descriptor.cpp
	dialogs.cpp
	gsmcodec.cpp
	libkvidcc.cpp
	marshal.cpp
	requests.cpp
	send.cpp
	thread.cpp
	utils.cpp
	voice.cpp
	window.cpp
)

IF(NOT DEFINED COMPILE_DISABLE_DCC_VIDEO) 
        LIST(APPEND kvidcc_SRCS 
                video.cpp 
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

