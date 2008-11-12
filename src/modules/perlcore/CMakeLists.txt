# CMakeLists for src/modules/perlcore

IF(UNIX)
	IF(APPLE)
		LIST(APPEND LIBS -lperl)
	ENDIF()
ELSE()
	# Assume windows
	LIST(APPEND LIBS -lperl58)
ENDIF()

SET(kviperlcore_SRCS
	libkviperlcore.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviperlcore_MOC_SRCS ${kviperlcore_MOC_HDRS})

SET(kvi_module_name kviperlcore)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
