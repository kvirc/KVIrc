# CMakeLists for src/modules/about
SET(kviabout_MOC_HDRS
	aboutdialog.h
)

SET(kviabout_SRCS
	aboutdialog.cpp
	libkviabout.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviabout_MOC_SRCS ${kviabout_MOC_HDRS})
SET(kvi_module_name kviabout)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
