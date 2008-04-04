# CMakeLists for src/modules/sharedfilewindow

SET(kvieditor_MOC_HDRS
	sharedfileswindow.h
)

SET(kvisharedfilewindow_SRCS
	libkvisharedfileswindow.cpp
	sharedfileswindow.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvisharedfilewindow_MOC_SRCS ${kvisharedfilewindow_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kvisharedfilewindow MODULE ${kvisharedfilewindow_SRCS} ${kvisharedfilewindow_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kvisharedfilewindow SHARED ${kvisharedfilewindow_SRCS} ${kvisharedfilewindow_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kvisharedfilewindow ???)
ENDIF()

# Install target
SET_TARGET_PROPERTIES(kvisharedfilewindow PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
IF(APPLE)
        INSTALL(TARGETS kvisharedfilewindow LIBRARY DESTINATION ${INSTALL_PREFIX}/Contents/Resources/modules/)
ELSE()
        INSTALL(TARGETS kvisharedfilewindow LIBRARY DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/)
ENDIF()
