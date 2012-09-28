# CMakeLists for src/modules/reguser

SET(kvireguser_SRCS
	libkvireguser.cpp
	RegisteredUserEntryDialog.cpp
	RegistrationWizard.cpp
	RegisteredUsersDialog.cpp
)

SET(kvi_module_name kvireguser)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
