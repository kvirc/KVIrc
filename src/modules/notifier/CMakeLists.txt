# CMakeLists for src/modules/notifier

SET(kvinotifier_MOC_HDRS
	notifierwindow.h
	notifierwindowtab.h
)

SET(kvinotifier_SRCS
	libkvinotifier.cpp
	notifierwindow.cpp
	notifiermessage.cpp
	notifierwindowtab.cpp
	notifierwindowborder.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvinotifier_MOC_SRCS ${kvinotifier_MOC_HDRS})

SET(kvi_module_name kvinotifier)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)

ADD_SUBDIRECTORY(pics)
