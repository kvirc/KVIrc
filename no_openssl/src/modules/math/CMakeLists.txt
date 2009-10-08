# CMakeLists for src/modules/math

SET(kvimath_SRCS
	libkvimath.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvimath_MOC_SRCS ${kvimath_MOC_HDRS})

SET(kvi_module_name kvimath)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
