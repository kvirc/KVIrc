# CMakeLists for src/modules/eventeditor

SET(kvieventeditor_MOC_HDRS
	eventeditor.h
)

SET(kvieventeditor_SRCS
	eventeditor.cpp
	libkvieventeditor.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvieventeditor_MOC_SRCS ${kvieventeditor_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kvieventeditor MODULE ${kvieventeditor_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kvieventeditor SHARED ${kvieventeditor_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kvieventeditor ???)
ENDIF()

# Install target
SET_TARGET_PROPERTIES(kvieventeditor PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
IF(APPLE)
        INSTALL(TARGETS kvieventeditor LIBRARY DESTINATION ${INSTALL_PREFIX}/Contents/Resources/modules/)
ELSE()
        INSTALL(TARGETS kvieventeditor LIBRARY DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/)
ENDIF()
