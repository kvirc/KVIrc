# CMakeLists for src/modules/editor

SET(kvieditor_MOC_HDRS
	scripteditor.h
)

SET(kvieditor_SRCS
	libkvieditor.cpp
	scripteditor.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvieditor_MOC_SRCS ${kvieditor_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kvieditor MODULE ${kvieditor_SRCS} ${kvieditor_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kvieditor SHARED ${kvieditor_SRCS} ${kvieditor_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kvieditor ???)
ENDIF()

# Install target
SET_TARGET_PROPERTIES(kvieditor PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
INSTALL(TARGETS kvieditor LIBRARY DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/)
