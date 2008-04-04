# CMakeLists for src/modules/regchan

SET(kviregchan_SRCS
	libkviregchan.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviregchan_MOC_SRCS ${kviregchan_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kviregchan MODULE ${kviregchan_SRCS} ${kviregchan_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kviregchan SHARED ${kviregchan_SRCS} ${kviregchan_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kviregchan ???)
ENDIF()

# Install target
SET_TARGET_PROPERTIES(kviregchan PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
IF(APPLE)
        INSTALL(TARGETS kviregchan LIBRARY DESTINATION ${INSTALL_PREFIX}/Contents/Resources/modules/)
ELSE()
        INSTALL(TARGETS kviregchan LIBRARY DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/)
ENDIF()
