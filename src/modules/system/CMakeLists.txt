# CMakeLists for src/modules/system

SET(kvieventeditor_MOC_HDRS
	plugin.h
)

SET(kvisystem_SRCS
	libkvisystem.cpp
	plugin.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvisystem_MOC_SRCS ${kvisystem_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kvisystem MODULE ${kvisystem_SRCS} ${kvisystem_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kvisystem SHARED ${kvisystem_SRCS} ${kvisystem_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kvisystem ???)
ENDIF()

# Install target
SET_TARGET_PROPERTIES(kvisystem PROPERTIES VERSION ${VERSION} SOVERSION ${MAJOR})
INSTALL(TARGETS kvisystem LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)

