# CMakeLists for src/modules/tmphighlight

SET(kvitmphighlight_SRCS
	libkvitmphighlight.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvitmphighlight_MOC_SRCS ${kvitmphighlight_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kvitmphighlight MODULE ${kvitmphighlight_SRCS} ${kvitmphighlight_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kvitmphighlight SHARED ${kvitmphighlight_SRCS} ${kvitmphighlight_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kvitmphighlight ???)
ENDIF()

# Install target
SET_TARGET_PROPERTIES(kvitmphighlight PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
INSTALL(TARGETS kvitmphighlight LIBRARY DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/)
