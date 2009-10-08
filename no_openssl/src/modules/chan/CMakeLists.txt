# CMakeLists for src/modules/chan

SET(kvichan_SRCS
	libkvichan.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvichan_MOC_SRCS ${kvichan_MOC_HDRS})

SET(kvi_module_name kvichan)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
