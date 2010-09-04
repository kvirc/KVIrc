# CMakeLists for src/modules/popup

SET(kvipopup_SRCS
	libkvipopup.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvipopup_MOC_SRCS ${kvipopup_MOC_HDRS})

SET(kvi_module_name kvipopup)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
