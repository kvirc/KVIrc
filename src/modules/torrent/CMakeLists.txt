# CMakeLists for src/modules/torrent

set(kvitorrent_SRCS
	libkvitorrent.cpp
	StatusBarApplet.cpp
	KTorrentDbusInterface.cpp
	TorrentInterface.cpp
)

set(kvi_module_name kvitorrent)
include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
