# CMakeLists for src/modules/torrent

SET(kvitorrent_SRCS
	libkvitorrent.cpp
	StatusBarApplet.cpp
	KTorrentDbusInterface.cpp
	TorrentInterface.cpp
)

SET(kvi_module_name kvitorrent)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
