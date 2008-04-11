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

SET(kvi_module_name kvinotifier)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
