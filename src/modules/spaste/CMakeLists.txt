# CMakeLists for src/modules/spaste

SET(kvieventeditor_MOC_HDRS
	controller.h
	libkvispaste.h
)

SET(kvispaste_SRCS
	libkvispaste.cpp
	controller.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvispaste_MOC_SRCS ${kvispaste_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kvispaste MODULE ${kvispaste_SRCS} ${kvispaste_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kvispaste SHARED ${kvispaste_SRCS} ${kvispaste_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kvispaste ???)
ENDIF()

# Install target
SET_TARGET_PROPERTIES(kvispaste PROPERTIES VERSION ${VERSION} SOVERSION ${MAJOR})
INSTALL(TARGETS kvispaste LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)
