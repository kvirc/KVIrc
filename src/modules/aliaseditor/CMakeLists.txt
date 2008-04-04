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
IF(UNIX)
	IF(APPLE)
		TARGET_LINK_LIBRARIES(kvialiaseditor kvilib ${LIBS})
		SET_TARGET_PROPERTIES(kvialiaseditor PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR} INSTALL_NAME_DIR @executable_path/../Frameworks)
		INSTALL(TARGETS kvialiaseditor LIBRARY DESTINATION ${INSTALL_PREFIX}/Contents/Resources/modules/)
	ELSE()
		# Assume linux
		SET_TARGET_PROPERTIES(kvialiaseditor PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
		INSTALL(TARGETS kvialiaseditor LIBRARY DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/)
	ENDIF()
ELSEIF(WIN32)
	SET_TARGET_PROPERTIES(kvialiaseditor PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
	#INSTALL(TARGETS kvialiaseditor LIBRARY DESTINATION ${INSTALL_PREFIX}/ ??? /${VERSION_BRANCH}/modules/)
ENDIF()
