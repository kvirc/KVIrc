# CMakeLists for src/modules/addon

SET(
	kviaddon_SRCS
	libkviaddon.cpp
	AddonManagementDialog.cpp
	PackAddonDialog.cpp
	AddonFunctions.cpp
	WebAddonInterfaceDialog.cpp
)

SET(kvi_module_name kviaddon)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
