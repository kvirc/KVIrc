# CMakeLists for src/

# Windows need the win32registrar component
IF(WIN32)
	ADD_SUBDIRECTORY(win32registrar)
ENDIF()

# Find subdirs
SUBDIRS(kvilib kvirc modules)