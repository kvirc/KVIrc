# CMakeLists for src/modules/logview

SET(kvilogview_MOC_HDRS
	logviewwidget.h
	logviewmdiwindow.h
)

SET(kvilogview_SRCS
	libkvilogview.cpp
	logviewwidget.cpp
	logviewmdiwindow.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvilogview_MOC_SRCS ${kvilogview_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kvilogview MODULE ${kvilogview_SRCS} ${kvilogview_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kvilogview SHARED ${kvilogview_SRCS} ${kvilogview_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kvilogview ???)
ENDIF()

# Install target
IF(UNIX)
	IF(APPLE)
		TARGET_LINK_LIBRARIES(kvilogview kvilib ${LIBS})
		SET_TARGET_PROPERTIES(kvilogview PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR} INSTALL_NAME_DIR @executable_path/../Frameworks)
		INSTALL(TARGETS kvilogview LIBRARY DESTINATION ${INSTALL_PREFIX}/Contents/Resources/modules/)
	ELSE()
		# Assume linux
		SET_TARGET_PROPERTIES(kvilogview PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
		INSTALL(TARGETS kvilogview LIBRARY DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/)
	ENDIF()
ELSEIF(WIN32)
	SET_TARGET_PROPERTIES(kvilogview PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
	#INSTALL(TARGETS kvilogview LIBRARY DESTINATION ${INSTALL_PREFIX}/ ??? /${VERSION_BRANCH}/modules/)
ENDIF()
