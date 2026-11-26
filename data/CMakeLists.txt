# CMakeLists for data/

add_subdirectory(applnk)
add_subdirectory(audio)
add_subdirectory(config)
add_subdirectory(defscript)
add_subdirectory(deftheme)
add_subdirectory(helppics)
add_subdirectory(man)
add_subdirectory(msgcolors)
add_subdirectory(pics)

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
