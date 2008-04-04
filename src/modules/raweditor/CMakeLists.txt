# CMakeLists for src/modules/raweditor

SET(kviraweditor_MOC_HDRS
	raweditor.h
)

SET(kviraweditor_SRCS
	libkviraweditor.cpp
	raweditor.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviraweditor_MOC_SRCS ${kviraweditor_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kviraweditor MODULE ${kviraweditor_SRCS} ${kviraweditor_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kviraweditor SHARED ${kviraweditor_SRCS} ${kviraweditor_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kviraweditor ???)
ENDIF()

# Install target
SET_TARGET_PROPERTIES(kviraweditor PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
IF(APPLE)
        INSTALL(TARGETS kviraweditor LIBRARY DESTINATION ${INSTALL_PREFIX}/Contents/Resources/modules/)
ELSE()
        INSTALL(TARGETS kviraweditor LIBRARY DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/)
ENDIF()
