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

SET(kviupnp_MOC_HDRS
	igdcontrolpoint.h
	Manager.h
	Service.h
	SsdpConnection.h
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviupnp_MOC_SRCS ${kviupnp_MOC_HDRS})

SET(kvi_module_name kviupnp)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
