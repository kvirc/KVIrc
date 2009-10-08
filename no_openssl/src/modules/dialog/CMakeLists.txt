# CMakeLists for src/modules/dialog

SET(kvidialog_MOC_HDRS
	libkvidialog.h
)

SET(kvidialog_SRCS
	libkvidialog.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvidialog_MOC_SRCS ${kvidialog_MOC_HDRS})

SET(kvi_module_name kvidialog)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
