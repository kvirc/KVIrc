# CMakeLists for win32build
SUBDIRS(translations)

#FIXME the nullsoft install scripts don't support the coexistence switch
SET(files
	KVIrc.nsi
	README.txt
)

IF(WIN32)
	INSTALL(FILES ${files} DESTINATION ${PROJECT_BINARY_DIR}/)
ENDIF()