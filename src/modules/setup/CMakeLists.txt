# CMakeLists for src/modules/setup

IF(WIN32)
	LIST(APPEND LIBS -lole32 -luuid)
ENDIF()

SET(kvisetup_MOC_HDRS
	SetupWizard.h
)

SET(kvisetup_SRCS
	libkvisetup.cpp
	SetupWizard.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvisetup_MOC_SRCS ${kvisetup_MOC_HDRS})

SET(kvi_module_name kvisetup)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
