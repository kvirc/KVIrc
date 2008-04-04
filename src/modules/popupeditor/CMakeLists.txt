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
IF(UNIX)
	IF(APPLE)
		TARGET_LINK_LIBRARIES(kvipopupeditor kvilib ${LIBS})
		SET_TARGET_PROPERTIES(kvipopupeditor PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR} INSTALL_NAME_DIR @executable_path/../Frameworks)
		INSTALL(TARGETS kvipopupeditor LIBRARY DESTINATION ${INSTALL_PREFIX}/Contents/Resources/modules/)
	ELSE()
		# Assume linux
		SET_TARGET_PROPERTIES(kvipopupeditor PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
		INSTALL(TARGETS kvipopupeditor LIBRARY DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/)
	ENDIF()
ELSEIF(WIN32)
	SET_TARGET_PROPERTIES(kvipopupeditor PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
	#INSTALL(TARGETS kvipopupeditor LIBRARY DESTINATION ${INSTALL_PREFIX}/ ??? /${VERSION_BRANCH}/modules/)
ENDIF()
