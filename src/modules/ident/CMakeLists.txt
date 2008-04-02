# CMakeLists for src/modules/ident

SET(kviident_MOC_HDRS
	libkviident.h
)

SET(kviident_SRCS
	libkviident.cpp
)

IF(WIN32)
	SET(LIBS ${LIBS} -lws2_32)
ENDIF(WIN32)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviident_MOC_SRCS ${kviident_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kviident MODULE ${kviident_SRCS} ${kviident_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kviident SHARED ${kviident_SRCS} ${kviident_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kviident ???)
ENDIF()

# Install target
SET_TARGET_PROPERTIES(kviident PROPERTIES VERSION ${VERSION} SOVERSION ${MAJOR})
INSTALL(TARGETS kviident LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)
