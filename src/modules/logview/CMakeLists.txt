# CMakeLists for src/modules/logview

SET(kvilogviewer_MOC_HDRS
	logviewwidget.h
	logviewmdiwindow.h
)

SET(kvilogviewer_SRCS
	libkvilogview.cpp
	logviewwidget.cpp
	logviewmdiwindow.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvilogviewer_MOC_SRCS ${kvilogviewer_MOC_HDRS})

SET(kvi_module_name kvilogviewer)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
