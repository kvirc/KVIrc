# CMakeLists for data/

# Find subdirs
SUBDIRS(config defscript deftheme helppics icons man mimelnk msgcolors pics)

IF(APPLE)
	# MacOS X
	ADD_SUBDIRECTORY(resources_mac)
ENDIF()