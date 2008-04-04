# CMakeLists for src/modules/about
SET(kviabout_MOC_HDRS
	aboutdialog.h
	libkviabout.h
)

SET(kviabout_SRCS
	libkviabout.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviabout_MOC_SRCS ${kviabout_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kviabout MODULE ${kviabout_SRCS} ${kviabout_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kviabout SHARED ${kviabout_SRCS} ${kviabout_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kviabout ???)
ENDIF()

# Install target
SET_TARGET_PROPERTIES(kviabout PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
IF(APPLE)
	INSTALL(TARGETS kviabout LIBRARY DESTINATION ${INSTALL_PREFIX}/Contents/Resources/modules/)
ELSE()
	INSTALL(TARGETS kviabout LIBRARY DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/)
ENDIF()

