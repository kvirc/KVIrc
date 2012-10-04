# CMakeLists for src/modules/upnp

set(kviupnp_SRCS
	igdcontrolpoint.cpp
	libkviupnp.cpp
	Manager.cpp
	RootService.cpp
	Service.cpp
	SsdpConnection.cpp
	WanConnectionService.cpp
	XmlFunctions.cpp
)



set(kvi_module_name kviupnp)
include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
