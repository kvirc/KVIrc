# CMakeLists for src/modules/toolbar

SET(kvitoolbar_SRCS
	libkvitoolbar.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvitoolbar_MOC_SRCS ${kvitoolbar_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kvitoolbar MODULE ${kvitoolbar_SRCS} ${kvitoolbar_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kvitoolbar SHARED ${kvitoolbar_SRCS} ${kvitoolbar_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kvitoolbar ???)
ENDIF()

# Install target
SET_TARGET_PROPERTIES(kvitoolbar PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
INSTALL(TARGETS kvitoolbar LIBRARY DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/)
