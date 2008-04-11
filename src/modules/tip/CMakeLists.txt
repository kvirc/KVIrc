# CMakeLists for src/modules/tip

SET(kvitip_MOC_HDRS
	libkvitip.h
)

SET(kvitip_SRCS
	libkvitip.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvitip_MOC_SRCS ${kvitip_MOC_HDRS})

SET(kvi_module_name kvitip)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
