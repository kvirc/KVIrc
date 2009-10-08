# CMakeLists for src/modules/context

SET(kvicontext_SRCS
	libkvicontext.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvicontext_MOC_SRCS ${kvicontext_MOC_HDRS})

SET(kvi_module_name kvicontext)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
