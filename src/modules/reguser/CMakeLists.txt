# CMakeLists for src/modules/reguser

set(kvireguser_SRCS
	libkvireguser.cpp
	RegisteredUserEntryDialog.cpp
	RegistrationWizard.cpp
	RegisteredUsersDialog.cpp
)

set(kvi_module_name kvireguser)
include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
