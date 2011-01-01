# CMakeLists for src/modules/logview

SUBDIRS(caps)

SET(kvilogview_MOC_HDRS
	LogViewWindow.h
)

SET(kvilogview_SRCS
	libkvilogview.cpp
	LogViewWidget.cpp
	LogViewWindow.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvilogview_MOC_SRCS ${kvilogview_MOC_HDRS})

SET(kvi_module_name kvilogview)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)

