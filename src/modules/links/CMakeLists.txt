# CMakeLists for src/modules/links

SET(kvilinks_MOC_HDRS
	LinksWindow.h
)

SET(kvilinks_SRCS
	libkvilinks.cpp
	LinksWindow.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvilinks_MOC_SRCS ${kvilinks_MOC_HDRS})

SET(kvi_module_name kvilinks)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
