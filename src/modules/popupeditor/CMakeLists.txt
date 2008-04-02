# CMakeLists for src/modules/popupeditor

SET(kvipopupeditor_MOC_HDRS
	popupeditor.h
)

SET(kvipopupeditor_SRCS
	libkvipopupeditor.cpp
	popupeditor.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvipopupeditor_MOC_SRCS ${kvipopupeditor_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kvipopupeditor MODULE ${kvipopupeditor_SRCS} ${kvipopupeditor_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kvipopupeditor SHARED ${kvipopupeditor_SRCS} ${kvipopupeditor_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kvipopupeditor ???)
ENDIF()

# Install target
SET_TARGET_PROPERTIES(kvipopupeditor PROPERTIES VERSION ${VERSION} SOVERSION ${MAJOR})
INSTALL(TARGETS kvipopupeditor LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)
