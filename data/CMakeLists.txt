# CMakeLists for data/

# Find subdirs
SUBDIRS(applnk config defscript deftheme helppics icons man mimetypes msgcolors pics)

IF(APPLE)
	# MacOS X
	ADD_SUBDIRECTORY(resources_mac)
ELSEIF(WIN32)
	ADD_SUBDIRECTORY(resources_win32)
ENDIF()
