# CMakeLists for src/modules/window

SET(kvieventeditor_MOC_HDRS
	userwindow.h
)

SET(kviwindow_SRCS
	libkviwindow.cpp
	userwindow.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviwindow_MOC_SRCS ${kviwindow_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kviwindow MODULE ${kviwindow_SRCS} ${kviwindow_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kviwindow SHARED ${kviwindow_SRCS} ${kviwindow_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kviwindow ???)
ENDIF()

# Install target
SET_TARGET_PROPERTIES(kviwindow PROPERTIES VERSION ${VERSION} SOVERSION ${MAJOR})
INSTALL(TARGETS kviwindow LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)
