# CMakeLists for src/modules/theme

SET(kvieventeditor_MOC_HDRS
	managementdialog.h
	packthemedialog.h
	savethemedialog.h
	themefunctions.h
)

SET(kvitheme_SRCS
	libkvitheme.cpp
	managementdialog.cpp
	packthemedialog.cpp
	savethemedialog.cpp
	themefunctions.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvitheme_MOC_SRCS ${kvitheme_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kvitheme MODULE ${kvitheme_SRCS} ${kvitheme_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kvitheme SHARED ${kvitheme_SRCS} ${kvitheme_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kvitheme ???)
ENDIF()

# Install target
SET_TARGET_PROPERTIES(kvitheme PROPERTIES VERSION ${VERSION} SOVERSION ${MAJOR})
INSTALL(TARGETS kvitheme LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)
