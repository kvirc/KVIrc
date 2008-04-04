# CMakeLists for src/modules/chan

SET(kvichan_SRCS
	libkvichan.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvichan_MOC_SRCS ${kvichan_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kvichan MODULE ${kvichan_SRCS} ${kvichan_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kvichan SHARED ${kvichan_SRCS} ${kvichan_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kvichan ???)
ENDIF()

# Install target
SET_TARGET_PROPERTIES(kvichan PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
IF(APPLE)
        INSTALL(TARGETS kvichan LIBRARY DESTINATION ${INSTALL_PREFIX}/Contents/Resources/modules/)
ELSE()
        INSTALL(TARGETS kvichan LIBRARY DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/)
ENDIF()
