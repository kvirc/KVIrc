# CMakeLists for src/modules/aliaseditor

SET(kvialiaseditor_MOC_HDRS
	aliaseditor.h
)

SET(kvialiaseditor_SRCS
	libkvialiaseditor.cpp
	aliaseditor.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvialiaseditor_MOC_SRCS ${kvialiaseditor_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kvialiaseditor MODULE ${kvialiaseditor_SRCS} ${kvialiaseditor_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kvialiaseditor SHARED ${kvialiaseditor_SRCS} ${kvialiaseditor_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kvialiaseditor ???)
ENDIF()

# Install target
SET_TARGET_PROPERTIES(kvialiaseditor PROPERTIES VERSION ${VERSION} SOVERSION ${MAJOR})
INSTALL(TARGETS kvialiaseditor LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)
