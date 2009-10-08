# CMakeLists for src/modules/toolbar

SET(kvitoolbar_SRCS
	libkvitoolbar.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvitoolbar_MOC_SRCS ${kvitoolbar_MOC_HDRS})

SET(kvi_module_name kvitoolbar)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
