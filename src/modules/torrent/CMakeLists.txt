# CMakeLists for src/modules/torrent

SET(kvitorrent_MOC_HDRS
	tc_ktorrentdbusinterface.h
	tc_statusbarapplet.h
)

SET(kvitorrent_SRCS
	libkvitorrent.cpp
	tc_statusbarapplet.cpp
	tc_ktorrentdbusinterface.cpp
	tc_interface.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvitorrent_MOC_SRCS ${kvitorrent_MOC_HDRS})

SET(kvi_module_name kvitorrent)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
