# CMakeLists for src/modules/addon

SET(kviaddon_MOC_HDRS
	managementdialog.h
)

SET(kviaddon_SRCS
	managementdialog.cpp
	libkviaddon.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviaddon_MOC_SRCS ${kviaddon_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kviaddon MODULE ${kviaddon_SRCS} ${kviaddon_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kviaddon SHARED ${kviaddon_SRCS} ${kviaddon_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kviaddon ???)
ENDIF()

# Install target
SET_TARGET_PROPERTIES(kviaddon PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
IF(APPLE)
        INSTALL(TARGETS kviaddon LIBRARY DESTINATION ${INSTALL_PREFIX}/Contents/Resources/modules/)
ELSE()
        INSTALL(TARGETS kviaddon LIBRARY DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/)
ENDIF()
