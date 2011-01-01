# CMakeLists for src/modules/help

SET(kvihelp_MOC_HDRS
	HelpWidget.h
	HelpWindow.h
	HelpIndex.h
)

SET(kvihelp_SRCS
	libkvihelp.cpp
	HelpWidget.cpp
	HelpWindow.cpp
	HelpIndex.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvihelp_MOC_SRCS ${kvihelp_MOC_HDRS})

SET(kvi_module_name kvihelp)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
