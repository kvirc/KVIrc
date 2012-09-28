# CMakeLists for src/modules/setup

IF(WIN32)
	LIST(APPEND LIBS -lole32 -luuid)
ENDIF()



SET(kvisetup_SRCS
	libkvisetup.cpp
	SetupWizard.cpp
)

SET(kvi_module_name kvisetup)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
