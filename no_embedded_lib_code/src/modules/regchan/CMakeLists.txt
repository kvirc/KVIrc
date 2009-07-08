# CMakeLists for src/modules/regchan

SET(kviregchan_SRCS
	libkviregchan.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviregchan_MOC_SRCS ${kviregchan_MOC_HDRS})

SET(kvi_module_name kviregchan)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
