# CMakeLists for src/modules/mircimport

SET(kvimircimport_MOC_HDRS
	libkvimircimport.h
)

SET(kvimircimport_SRCS
	libkvimircimport.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvimircimport_MOC_SRCS ${kvimircimport_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kvimircimport MODULE ${kvimircimport_SRCS} ${kvimircimport_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kvimircimport SHARED ${kvimircimport_SRCS} ${kvimircimport_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kvimircimport ???)
ENDIF()

# Install target
SET_TARGET_PROPERTIES(kvimircimport PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
IF(APPLE)
        INSTALL(TARGETS kvimircimport LIBRARY DESTINATION ${INSTALL_PREFIX}/Contents/Resources/modules/)
ELSE()
        INSTALL(TARGETS kvimircimport LIBRARY DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/)
ENDIF()
