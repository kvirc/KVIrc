# CMakeLists for src/modules/setup

if(MINGW)
	list(APPEND LIBS -lole32 -luuid)
endif()



set(kvisetup_SRCS
	libkvisetup.cpp
	SetupWizard.cpp
)

set(kvi_module_name kvisetup)
include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
