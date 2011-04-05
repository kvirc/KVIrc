# CMakeLists for data/

# Find subdirs
SUBDIRS(applnk audio config defscript deftheme helppics icons man msgcolors pics)

IF(APPLE)
	# MacOS X
	ADD_SUBDIRECTORY(resources_mac)
ELSEIF(WIN32)
	ADD_SUBDIRECTORY(resources_win32)
ELSE()
	ADD_SUBDIRECTORY(mimetypes)
	ADD_SUBDIRECTORY(protocols)
ENDIF()
