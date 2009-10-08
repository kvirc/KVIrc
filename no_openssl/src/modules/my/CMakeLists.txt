# CMakeLists for src/modules/my

SET(kvimy_MOC_HDRS
	idle.h
)

SET(kvimy_SRCS
	libkvimy.cpp
	idle_x11.cpp
	idle_mac.cpp
	idle.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvimy_MOC_SRCS ${kvimy_MOC_HDRS})

SET(kvi_module_name kvimy)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
