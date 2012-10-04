# CMakeLists for src/modules/channelsjoin

set(kvichannelsjoin_SRCS
	libkvichannelsjoin.cpp
	ChannelsJoinDialog.cpp
)

set(kvi_module_name kvichannelsjoin)
include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
