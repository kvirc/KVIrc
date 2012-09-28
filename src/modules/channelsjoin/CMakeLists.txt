# CMakeLists for src/modules/channelsjoin

SET(kvichannelsjoin_SRCS
	libkvichannelsjoin.cpp
	ChannelsJoinDialog.cpp
)

SET(kvi_module_name kvichannelsjoin)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
