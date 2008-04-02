# CMakeLists for src/modules/snd

IF(WIN32)
	LIST(APPEND LIBS -lwinmm)
ENDIF()

SET(kvieditor_MOC_HDRS
	libkvisnd.h
)

SET(kvisnd_SRCS
	libkvisnd.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvisnd_MOC_SRCS ${kvisnd_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kvisnd MODULE ${kvisnd_SRCS} ${kvisnd_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kvisnd SHARED ${kvisnd_SRCS} ${kvisnd_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kvisnd ???)
ENDIF()

# Install target
SET_TARGET_PROPERTIES(kvisnd PROPERTIES VERSION ${VERSION} SOVERSION ${MAJOR})
INSTALL(TARGETS kvisnd LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)
