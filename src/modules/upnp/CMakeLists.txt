# CMakeLists for src/modules/upnp

SET(kviupnp_SRCS
	igdcontrolpoint.cpp
	libkviupnp.cpp
	Manager.cpp
	RootService.cpp
	Service.cpp
	SsdpConnection.cpp
	WanConnectionService.cpp
	XmlFunctions.cpp
)



SET(kvi_module_name kviupnp)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
