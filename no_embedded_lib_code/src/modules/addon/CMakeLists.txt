# CMakeLists for src/modules/addon

SET(kviaddon_MOC_HDRS
	managementdialog.h
	packaddondialog.h
)

SET(kviaddon_SRCS
	libkviaddon.cpp
	managementdialog.cpp
	packaddondialog.cpp
	addonfunctions.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviaddon_MOC_SRCS ${kviaddon_MOC_HDRS})

SET(kvi_module_name kviaddon)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
