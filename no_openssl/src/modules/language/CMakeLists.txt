# CMakeLists for src/modules/language

SET(kvilanguage_SRCS
	libkvilanguage.cpp
	detector.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvilanguage_MOC_SRCS ${kvilanguage_MOC_HDRS})

SET(kvi_module_name kvilanguage)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
