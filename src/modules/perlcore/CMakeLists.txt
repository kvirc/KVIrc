# CMakeLists for src/modules/perlcore

# From qmake
#win32{
#	INCLUDEPATH += $(PERLDIR)/lib/CORE/
#	LIBS += -lperl58
#}
#
#mac {
#	INCLUDEPATH += $(PERLDIR)/CORE/
#	LIBS += -lperl -L$(PERLDIR)/CORE/
#}

IF(UNIX)
	IF(APPLE)
		LIST(APPEND LIBS -lperl)
	ENDIF()
ELSE()
	# Assume windows
	LIST(APPEND LIBS -lperl58)
ENDIF()

SET(kviperlcore_MOC_HDRS
	perlcoreinterface.h
)

SET(kviperlcore_SRCS
	libkviperlcore.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviperlcore_MOC_SRCS ${kviperlcore_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kviperlcore MODULE ${kviperlcore_SRCS} ${kviperlcore_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kviperlcore SHARED ${kviperlcore_SRCS} ${kviperlcore_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kviperlcore ???)
ENDIF()

# Install target
IF(UNIX)
	IF(APPLE)
		TARGET_LINK_LIBRARIES(kviperlcore kvilib ${LIBS})
		SET_TARGET_PROPERTIES(kviperlcore PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR} INSTALL_NAME_DIR @executable_path/../Frameworks)
		INSTALL(TARGETS kviperlcore LIBRARY DESTINATION ${INSTALL_PREFIX}/Contents/Resources/modules/)
	ELSE()
		# Assume linux
		SET_TARGET_PROPERTIES(kviperlcore PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
		INSTALL(TARGETS kviperlcore LIBRARY DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/)
	ENDIF()
ELSE()
	# Windows
	SET_TARGET_PROPERTIES(kviperlcore PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
	#INSTALL(TARGETS kviperlcore LIBRARY DESTINATION ${INSTALL_PREFIX}/ ??? /${VERSION_BRANCH}/modules/)
ENDIF()
