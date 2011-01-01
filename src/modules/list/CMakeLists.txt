# CMakeLists for src/modules/list

SET(kvilist_MOC_HDRS
	ListWindow.h
)

SET(kvilist_SRCS
	libkvilist.cpp
	ListWindow.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvilist_MOC_SRCS ${kvilist_MOC_HDRS})

SET(kvi_module_name kvilist)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
