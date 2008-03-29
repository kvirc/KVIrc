# CMakeLists for src/modules/dcc

SET(kvidcc_MOC_HDRS
	adpcmcodec.h
	broker.h
	canvas.h
	canvaswidget.h
	chat.h
	codec.h
	descriptor.h
	dialogs.h
	gsmcodec.h
	marshal.h
	send.h
	thread.h
	utils.h
	voice.h
	window.h
)

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

#IF(WIN32)
#	SET(LIBS ${LIBS} -lws2_32)
#ENDIF(WIN32)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvidcc_MOC_SRCS ${kvidcc_MOC_HDRS})

# Create library
ADD_LIBRARY(kvidcc SHARED ${kvidcc_SRCS} ${kvidcc_MOC_SRCS})
#TARGET_LINK_LIBRARIES(kvilib kvidcc)

# Install target
SET_TARGET_PROPERTIES(kvidcc PROPERTIES VERSION ${KVI_VERSION} SOVERSION ${KVI_MAJOR})
INSTALL(TARGETS kvidcc LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)