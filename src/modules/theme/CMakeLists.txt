# CMakeLists for src/modules/theme

SET(kvitheme_MOC_HDRS
	managementdialog.h
	PackThemeDialog.h
	SaveThemeDialog.h
)

SET(kvitheme_SRCS
	libkvitheme.cpp
	managementdialog.cpp
	PackThemeDialog.cpp
	SaveThemeDialog.cpp
	ThemeFunctions.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvitheme_MOC_SRCS ${kvitheme_MOC_HDRS})

SET(kvi_module_name kvitheme)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
