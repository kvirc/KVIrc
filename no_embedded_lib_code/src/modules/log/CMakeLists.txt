# CMakeLists for src/modules/log

SET(kvilog_SRCS
	libkvilog.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvilog_MOC_SRCS ${kvilog_MOC_HDRS})
SET(kvi_module_name kvilog)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
