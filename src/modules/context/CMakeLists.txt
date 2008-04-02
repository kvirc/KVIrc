# CMakeLists for src/modules/context

SET(kvicontext_SRCS
	libkvicontext.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvicontext_MOC_SRCS ${kvicontext_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kvicontext MODULE ${kvicontext_SRCS} ${kvicontext_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kvicontext SHARED ${kvicontext_SRCS} ${kvicontext_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kvicontext ???)
ENDIF()

# Install target
SET_TARGET_PROPERTIES(kvicontext PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
INSTALL(TARGETS kvicontext LIBRARY DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/)
