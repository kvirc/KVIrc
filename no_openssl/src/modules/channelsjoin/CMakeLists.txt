# CMakeLists for src/modules/channelsjoin

SET(kvichannelsjoin_SRCS
	libkvichannelsjoin.cpp
	channelsjoinwindow.cpp
)
SET(kvichannelsjoin_MOC_HDRS
	channelsjoinwindow.h
)
# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvichannelsjoin_MOC_SRCS ${kvichannelsjoin_MOC_HDRS})

SET(kvi_module_name kvichannelsjoin)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
