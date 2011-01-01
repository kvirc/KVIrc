# CMakeLists for src/modules/codetester

SET(kvicodetester_MOC_HDRS
	CodeTesterWindow.h
)

SET(kvicodetester_SRCS
	libkvicodetester.cpp
	CodeTesterWindow.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvicodetester_MOC_SRCS ${kvicodetester_MOC_HDRS})

SET(kvi_module_name kvicodetester)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
