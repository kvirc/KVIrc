# CMakeLists for src/modules/avatar

SET(kviavatar_MOC_HDRS
	libkviavatar.h
)

SET(kviavatar_SRCS
	libkviavatar.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviavatar_MOC_SRCS ${kviavatar_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kviavatar MODULE ${kviavatar_SRCS} ${kviavatar_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kviavatar SHARED ${kviavatar_SRCS} ${kviavatar_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kviavatar ???)
ENDIF()

# Install target
SET_TARGET_PROPERTIES(kviavatar PROPERTIES VERSION ${VERSION} SOVERSION ${MAJOR})
INSTALL(TARGETS kviavatar LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)
