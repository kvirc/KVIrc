# CMakeLists for src/modules/socketspy

SET(kvieditor_MOC_HDRS
	socketspywindow.h
)

SET(kvisocketspy_SRCS
	libkvisocketspy.cpp
	socketspywindow.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvisocketspy_MOC_SRCS ${kvisocketspy_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kvisocketspy MODULE ${kvisocketspy_SRCS} ${kvisocketspy_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kvisocketspy SHARED ${kvisocketspy_SRCS} ${kvisocketspy_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kvisocketspy ???)
ENDIF()

# Install target
SET_TARGET_PROPERTIES(kvisocketspy PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
INSTALL(TARGETS kvisocketspy LIBRARY DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/)
