# CMakeLists for src/modules/trayicon

SET(kvitrayicon_MOC_HDRS
	libkvitrayicon.h
)

SET(kvitrayicon_SRCS
	libkvitrayicon.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvitrayicon_MOC_SRCS ${kvitrayicon_MOC_HDRS})

SET(kvi_module_name kvitrayicon)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
