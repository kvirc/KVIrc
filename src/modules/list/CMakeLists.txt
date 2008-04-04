# CMakeLists for src/modules/list

SET(kvilist_MOC_HDRS
	listwindow.h
)

SET(kvilist_SRCS
	libkvilist.cpp
	listwindow.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvilist_MOC_SRCS ${kvilist_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kvilist MODULE ${kvilist_SRCS} ${kvilist_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kvilist SHARED ${kvilist_SRCS} ${kvilist_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kvilist ???)
ENDIF()

# Install target
SET_TARGET_PROPERTIES(kvilist PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
IF(APPLE)
        INSTALL(TARGETS kvilist LIBRARY DESTINATION ${INSTALL_PREFIX}/Contents/Resources/modules/)
ELSE()
        INSTALL(TARGETS kvilist LIBRARY DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/)
ENDIF()
