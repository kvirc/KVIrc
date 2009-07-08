# CMakeLists for src/modules/url

SUBDIRS(caps)

SET(kviurl_MOC_HDRS
	libkviurl.h
)

SET(kviurl_SRCS
	libkviurl.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviurl_MOC_SRCS ${kviurl_MOC_HDRS})

SET(kvi_module_name kviurl)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
