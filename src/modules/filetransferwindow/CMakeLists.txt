# CMakeLists for src/modules/filetransferwindow

SET(kvifiletransferwindow_MOC_HDRS
	filetransferwindow.h
)

SET(kvifiletransferwindow_SRCS
	libkvifiletransferwindow.cpp
	filetransferwindow.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvifiletransferwindow_MOC_SRCS ${kvifiletransferwindow_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kvifiletransferwindow MODULE ${kvifiletransferwindow_SRCS} ${kvifiletransferwindow_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kvifiletransferwindow SHARED ${kvifiletransferwindow_SRCS} ${kvifiletransferwindow_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kvifiletransferwindow ???)
ENDIF()

# Install target
SET_TARGET_PROPERTIES(kvifiletransferwindow PROPERTIES VERSION ${VERSION} SOVERSION ${MAJOR})
INSTALL(TARGETS kvifiletransferwindow LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)
