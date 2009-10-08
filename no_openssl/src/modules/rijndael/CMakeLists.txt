# CMakeLists for src/modules/rijndael

SUBDIRS(caps)

SET(kvirijndael_MOC_HDRS
	libkvirijndael.h
)

SET(kvirijndael_SRCS
	libkvirijndael.cpp
	rijndael.cpp
	ablowfish.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvirijndael_MOC_SRCS ${kvirijndael_MOC_HDRS})

SET(ADDITIONAL_LINK_FLAGS "${ADDITIONAL_LINK_FLAGS} -fno-strict-aliasing")

SET(kvi_module_name kvirijndael)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
