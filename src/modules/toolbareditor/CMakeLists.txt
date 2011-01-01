# CMakeLists for src/modules/toolbareditor

SET(kvitoolbareditor_MOC_HDRS
	CustomizeToolBarsDialog.h
)

SET(kvitoolbareditor_SRCS
	libkvitoolbareditor.cpp
	CustomizeToolBarsDialog.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvitoolbareditor_MOC_SRCS ${kvitoolbareditor_MOC_HDRS})

SET(kvi_module_name kvitoolbareditor)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
