# CMakeLists for src/modules/window

SET(kviwindow_MOC_HDRS
	UserWindow.h
)

SET(kviwindow_SRCS
	libkviwindow.cpp
	UserWindow.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviwindow_MOC_SRCS ${kviwindow_MOC_HDRS})

SET(kvi_module_name kviwindow)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
