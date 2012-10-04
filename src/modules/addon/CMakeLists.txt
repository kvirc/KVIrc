# CMakeLists for src/modules/addon

set(
	kviaddon_SRCS
	libkviaddon.cpp
	AddonManagementDialog.cpp
	PackAddonDialog.cpp
	AddonFunctions.cpp
	WebAddonInterfaceDialog.cpp
)

set(kvi_module_name kviaddon)
include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
