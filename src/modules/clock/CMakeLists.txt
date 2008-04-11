# CMakeLists for src/modules/clock

SET(kviclock_MOC_HDRS
	libkviclock.h
)

SET(kviclock_SRCS
	libkviclock.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviclock_MOC_SRCS ${kviclock_MOC_HDRS})

SET(kvi_module_name kviclock)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
