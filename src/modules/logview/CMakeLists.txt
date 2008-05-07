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

SET(kvi_module_name kvilogview)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)

