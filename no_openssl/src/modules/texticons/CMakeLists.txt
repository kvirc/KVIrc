# CMakeLists for src/modules/texticons

SET(kvitexticons_SRCS
	libkvitexticons.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvitexticons_MOC_SRCS ${kvitexticons_MOC_HDRS})

SET(kvi_module_name kvitexticons)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
