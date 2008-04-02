# CMakeLists for src/modules/math

SET(kvimath_SRCS
	libkvimath.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvimath_MOC_SRCS ${kvimath_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kvimath MODULE ${kvimath_SRCS} ${kvimath_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kvimath SHARED ${kvimath_SRCS} ${kvimath_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kvimath ???)
ENDIF()

# Install target
SET_TARGET_PROPERTIES(kvimath PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
INSTALL(TARGETS kvimath LIBRARY DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/)
