# CMakeLists for src/modules/tip

SET(kvieventeditor_MOC_HDRS
	libkvitip.h
)

SET(kvitip_SRCS
	libkvitip.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvitip_MOC_SRCS ${kvitip_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kvitip MODULE ${kvitip_SRCS} ${kvitip_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kvitip SHARED ${kvitip_SRCS} ${kvitip_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kvitip ???)
ENDIF()

# Install target
SET_TARGET_PROPERTIES(kvitip PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
IF(APPLE)
        INSTALL(TARGETS kvitip LIBRARY DESTINATION ${INSTALL_PREFIX}/Contents/Resources/modules/)
ELSE()
        INSTALL(TARGETS kvitip LIBRARY DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/)
ENDIF()
