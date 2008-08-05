# CMakeLists for src/modules/rijndael

SUBDIRS(caps)

SET(kvirijndael__MOC_HDRS
	libkvirijndael.h
	rijndael.h
	ablowfish.h
)

SET(kvirijndael_SRCS
	libkvirijndael.cpp
	rijndael.cpp
	ablowfish.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvirijndael_MOC_SRCS ${kvirijndael_MOC_HDRS} "-DCOMPILE_CRYPT_SUPPORT")

SET(kvi_module_name kvirijndael)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
