# CMakeLists for src/modules/notifier

SET(kvinotifier_MOC_HDRS
	notifiersettings.h
	notifierwindow.h
	notifiermessage.h
	notifierwindowtabs.h
	notifierwindowbody.h
	notifierwindowborder.h
	notifierwindowprogressbar.h
)

SET(kvinotifier_SRCS
	libkvinotifier.cpp
	notifierwindow.cpp
	notifiermessage.cpp
	notifierwindowtabs.cpp
	notifierwindowbody.cpp
	notifierwindowborder.cpp
	notifierwindowprogressbar.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvinotifier_MOC_SRCS ${kvinotifier_MOC_HDRS})

# Create library
IF(UNIX)
	IF(APPLE)
		ADD_LIBRARY(kvinotifier MODULE ${kvinotifier_SRCS} ${kvinotifier_MOC_SRCS})
	ELSE()
		# Assume linux
		ADD_LIBRARY(kvinotifier SHARED ${kvinotifier_SRCS} ${kvinotifier_MOC_SRCS})
	ENDIF()
ELSE()
	# Assume windows
	#ADD_LIBRARY(kvinotifier ???)
ENDIF()

# Install target
SET_TARGET_PROPERTIES(kvinotifier PROPERTIES VERSION ${VERSION_RELEASE} SOVERSION ${VERSION_MAJOR})
IF(APPLE)
        INSTALL(TARGETS kvinotifier LIBRARY DESTINATION ${INSTALL_PREFIX}/Contents/Resources/modules/)
ELSE()
        INSTALL(TARGETS kvinotifier LIBRARY DESTINATION ${INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/modules/)
ENDIF()
