# CMakeLists for src/modules/avatar

SET(kviavatar_MOC_HDRS
	libkviavatar.h
)

SET(kviavatar_SRCS
	libkviavatar.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviavatar_MOC_SRCS ${kviavatar_MOC_HDRS})

SET(kvi_module_name kviavatar)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
