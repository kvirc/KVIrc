# CMakeLists for src/modules/term

SET(kviterm_MOC_HDRS
	termwidget.h
	termwindow.h
)

SET(kviterm_SRCS
	libkviterm.cpp
	termwidget.cpp
	termwindow.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviterm_MOC_SRCS ${kviterm_MOC_HDRS})

SET(kvi_module_name kviterm)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
