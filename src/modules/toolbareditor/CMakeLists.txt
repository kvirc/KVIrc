# CMakeLists for src/modules/toolbareditor

SET(kvieventeditor_MOC_HDRS
	toolbareditor.h
)

SET(kvitoolbareditor_SRCS
	libkvitoolbareditor.cpp
	toolbareditor.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvitoolbareditor_MOC_SRCS ${kvitoolbareditor_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kvitoolbareditor MODULE ${kvitoolbareditor_SRCS} ${kvitoolbareditor_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kvitoolbareditor SHARED ${kvitoolbareditor_SRCS} ${kvitoolbareditor_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kvitoolbareditor ???)
ENDIF()

# Install target
SET_TARGET_PROPERTIES(kvitoolbareditor PROPERTIES VERSION ${VERSION} SOVERSION ${MAJOR})
INSTALL(TARGETS kvitoolbareditor LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)
