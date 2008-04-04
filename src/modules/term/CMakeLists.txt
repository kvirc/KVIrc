# CMakeLists for src/modules/term

SET(kvieventeditor_MOC_HDRS
	termwidget.h
	termwindow.h
)

SET(kviterm_SRCS
	libkviterm.cpp
	termwidget.cpp
	termwindow.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviterm_MOC_SRCS ${kviterm_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kviterm MODULE ${kviterm_SRCS} ${kviterm_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kviterm SHARED ${kviterm_SRCS} ${kviterm_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kviterm ???)
ENDIF()

# Install target
SET_TARGET_PROPERTIES(kviterm PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
IF(APPLE)
        INSTALL(TARGETS kviterm LIBRARY DESTINATION ${INSTALL_PREFIX}/Contents/Resources/modules/)
ELSE()
        INSTALL(TARGETS kviterm LIBRARY DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/)
ENDIF()
