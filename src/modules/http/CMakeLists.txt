# CMakeLists for src/modules/http

SET(kvihttp_MOC_HDRS
	httpfiletransfer.h
)

SET(kvihttp_SRCS
	libkvihttp.cpp
	httpfiletransfer.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvihttp_MOC_SRCS ${kvihttp_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kvihttp MODULE ${kvihttp_SRCS} ${kvihttp_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kvihttp SHARED ${kvihttp_SRCS} ${kvihttp_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kvihttp ???)
ENDIF()

# Install target
SET_TARGET_PROPERTIES(kvihttp PROPERTIES VERSION ${VERSION} SOVERSION ${MAJOR})
INSTALL(TARGETS kvihttp LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)
