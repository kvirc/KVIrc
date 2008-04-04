# CMakeLists for src/modules/reguser

SET(kvieditor_MOC_HDRS
	edituser.h
	wizard.h
	dialog.h
)

SET(kvireguser_SRCS
	libkvireguser.cpp
	edituser.cpp
	wizard.cpp
	dialog.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvireguser_MOC_SRCS ${kvireguser_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kvireguser MODULE ${kvireguser_SRCS} ${kvireguser_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kvireguser SHARED ${kvireguser_SRCS} ${kvireguser_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kvireguser ???)
ENDIF()

# Install target
SET_TARGET_PROPERTIES(kvireguser PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
IF(APPLE)
        INSTALL(TARGETS kvireguser LIBRARY DESTINATION ${INSTALL_PREFIX}/Contents/Resources/modules/)
ELSE()
        INSTALL(TARGETS kvireguser LIBRARY DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/)
ENDIF()
