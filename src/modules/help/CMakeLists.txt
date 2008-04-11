# CMakeLists for src/modules/help

SET(kvihelp_MOC_HDRS
	helpwidget.h
	helpwindow.h
	index.h
)

SET(kvihelp_SRCS
	libkvihelp.cpp
	helpwidget.cpp
	helpwindow.cpp
	index.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvihelp_MOC_SRCS ${kvihelp_MOC_HDRS})

SET(kvi_module_name kvihelp)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
