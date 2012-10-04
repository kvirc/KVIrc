# CMakeLists for src/modules/theme

set(kvitheme_SRCS
	libkvitheme.cpp
	WebThemeInterfaceDialog.cpp
	ThemeManagementDialog.cpp
	PackThemeDialog.cpp
	SaveThemeDialog.cpp
	ThemeFunctions.cpp
)

set(kvi_module_name kvitheme)
include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
