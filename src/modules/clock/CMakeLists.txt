# CMakeLists for src/modules/clock

SET(kviclock_MOC_HDRS
	libkviclock.h
)

SET(kviclock_SRCS
	libkviclock.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviclock_MOC_SRCS ${kviclock_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kviclock MODULE ${kviclock_SRCS} ${kviclock_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kviclock SHARED ${kviclock_SRCS} ${kviclock_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kviclock ???)
ENDIF()

# Install target
SET_TARGET_PROPERTIES(kviclock PROPERTIES VERSION ${VERSION} SOVERSION ${MAJOR})
INSTALL(TARGETS kviclock LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)
