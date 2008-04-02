# CMakeLists for src/modules/iograph

SET(kviiograph_MOC_HDRS
	libkviiograph.h
)

SET(kviiograph_SRCS
	libkviiograph.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviiograph_MOC_SRCS ${kviiograph_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kviiograph MODULE ${kviiograph_SRCS} ${kviiograph_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kviiograph SHARED ${kviiograph_SRCS} ${kviiograph_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kviiograph ???)
ENDIF()

# Install target
SET_TARGET_PROPERTIES(kviiograph PROPERTIES VERSION ${VERSION} SOVERSION ${MAJOR})
INSTALL(TARGETS kviiograph LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)
