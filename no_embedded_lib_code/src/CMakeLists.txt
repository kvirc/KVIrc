# CMakeLists for src/

IF(WIN32)
	# We need this defined when compiling with cmake
	ADD_DEFINITIONS(-DCOMPILE_USE_STANDALONE_MOC_SOURCES)
ENDIF()

# Find subdirs
SUBDIRS(kvilib kvirc modules)
