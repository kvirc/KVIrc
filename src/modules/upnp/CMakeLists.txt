# CMakeLists for src/modules/upnp

SET(kviupnp_SRCS
	igdcontrolpoint.cpp
	libkviupnp.cpp
	manager.cpp
	rootservice.cpp
	service.cpp
	ssdpconnection.cpp
	wanconnectionservice.cpp
	xmlfunctions.cpp
)

SET(kviupnp_MOC_HDRS
	igdcontrolpoint.h
	manager.h
	service.h
	ssdpconnection.h
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviupnp_MOC_SRCS ${kviupnp_MOC_HDRS})

SET(kvi_module_name kviupnp)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
