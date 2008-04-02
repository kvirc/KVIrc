# CMakeLists for src/modules/url

SET(kvieventeditor_MOC_HDRS
	libkviurl.h
	icons.h
)

SET(kviurl_SRCS
	libkviurl.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviurl_MOC_SRCS ${kviurl_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kviurl MODULE ${kviurl_SRCS} ${kviurl_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kviurl SHARED ${kviurl_SRCS} ${kviurl_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kviurl ???)
ENDIF()

# Install target
SET_TARGET_PROPERTIES(kviurl PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
INSTALL(TARGETS kviurl LIBRARY DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/)
