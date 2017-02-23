# CMakeLists for data/

# Find subdirs
subdirs(applnk audio config defscript deftheme helppics man msgcolors pics)

if(APPLE)
	# macOS
	add_subdirectory(resources_mac)
elseif(WIN32)
	add_subdirectory(resources_win32)
elseif(UNIX AND NOT APPLE)
	add_subdirectory(icons)
	add_subdirectory(mimetypes)
	add_subdirectory(protocols)
endif()
