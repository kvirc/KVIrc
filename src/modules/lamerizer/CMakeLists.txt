# CMakeLists for src/modules/lamerizer

SET(kvilamerizer_MOC_HDRS
	libkvilamerizer.h
)

SET(kvilamerizer_SRCS
	libkvilamerizer.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvilamerizer_MOC_SRCS ${kvilamerizer_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kvilamerizer MODULE ${kvilamerizer_SRCS} ${kvilamerizer_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kvilamerizer SHARED ${kvilamerizer_SRCS} ${kvilamerizer_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kvilamerizer ???)
ENDIF()

# Install target
SET_TARGET_PROPERTIES(kvilamerizer PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
IF(APPLE)
        INSTALL(TARGETS kvilamerizer LIBRARY DESTINATION ${INSTALL_PREFIX}/Contents/Resources/modules/)
ELSE()
        INSTALL(TARGETS kvilamerizer LIBRARY DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/)
ENDIF()
