# CMakeLists for src/modules/popupeditor

SET(kvipopupeditor_MOC_HDRS
	popupeditor.h
)

SET(kvipopupeditor_SRCS
	libkvipopupeditor.cpp
	popupeditor.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvipopupeditor_MOC_SRCS ${kvipopupeditor_MOC_HDRS})

SET(kvi_module_name kvipopupeditor)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
