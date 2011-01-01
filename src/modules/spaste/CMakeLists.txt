# CMakeLists for src/modules/spaste

SET(kvispaste_MOC_HDRS
	SlowPasteController.h
)

SET(kvispaste_SRCS
	libkvispaste.cpp
	SlowPasteController.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvispaste_MOC_SRCS ${kvispaste_MOC_HDRS})

SET(kvi_module_name kvispaste)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
