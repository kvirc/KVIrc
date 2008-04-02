# CMakeLists for src/modules/channelsjoin

SET(kvichannelsjoin_SRCS
	libkvichannelsjoin.cpp
	channelsjoinwindow.cpp
)
SET(kvichannelsjoin_MOC_HDRS
	channelsjoinwindow.h
)
# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvichannelsjoin_MOC_SRCS ${kvichannelsjoin_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kvichannelsjoin MODULE ${kvichannelsjoin_SRCS} ${kvichannelsjoin_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kvichannelsjoin SHARED ${kvichannelsjoin_SRCS} ${kvichannelsjoin_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kvichannelsjoin ???)
ENDIF()

# Install target
SET_TARGET_PROPERTIES(kvichannelsjoin PROPERTIES VERSION ${VERSION} SOVERSION ${MAJOR})
INSTALL(TARGETS kvichannelsjoin LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)
