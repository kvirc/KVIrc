# CMakeLists for src/modules/action

SET(kviaction_SRCS
	libkviaction.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviaction_MOC_SRCS ${kviaction_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kviaction MODULE ${kviaction_SRCS} ${kviaction_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kviaction SHARED ${kviaction_SRCS} ${kviaction_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kviaction ???)
ENDIF()

# Install target
SET_TARGET_PROPERTIES(kviaction PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
IF(APPLE)
        INSTALL(TARGETS kviaction LIBRARY DESTINATION ${INSTALL_PREFIX}/Contents/Resources/modules/)
ELSE()
        INSTALL(TARGETS kviaction LIBRARY DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/)
ENDIF()
