# CMakeLists for src/modules/socketspy

SET(kvisocketspy_MOC_HDRS
	SocketSpyWindow.h
)

SET(kvisocketspy_SRCS
	libkvisocketspy.cpp
	SocketSpyWindow.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvisocketspy_MOC_SRCS ${kvisocketspy_MOC_HDRS})

SET(kvi_module_name kvisocketspy)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
