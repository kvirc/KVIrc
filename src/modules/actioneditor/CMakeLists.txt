# CMakeLists for src/modules/actioneditor

SET(kviactioneditor_MOC_HDRS
	actioneditor.h
)

SET(kviactioneditor_SRCS
	actioneditor.cpp
	libkviactioneditor.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviactioneditor_MOC_SRCS ${kviactioneditor_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kviactioneditor MODULE ${kviactioneditor_SRCS} ${kviactioneditor_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kviactioneditor SHARED ${kviactioneditor_SRCS} ${kviactioneditor_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kviactioneditor ???)
ENDIF()

# Install target
SET_TARGET_PROPERTIES(kviactioneditor PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
INSTALL(TARGETS kviactioneditor LIBRARY DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/)
