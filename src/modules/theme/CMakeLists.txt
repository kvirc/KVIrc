# CMakeLists for src/modules/theme

SET(kvitheme_SRCS
	libkvitheme.cpp
	WebThemeInterfaceDialog.cpp
	ThemeManagementDialog.cpp
	PackThemeDialog.cpp
	SaveThemeDialog.cpp
	ThemeFunctions.cpp
)

SET(kvi_module_name kvitheme)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
