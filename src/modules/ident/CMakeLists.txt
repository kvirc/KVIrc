# CMakeLists for src/modules/ident

SET(kviident_MOC_HDRS
	libkviident.h
)

SET(kviident_SRCS
	libkviident.cpp
)

IF(WIN32)
	SET(LIBS ${LIBS} -lws2_32)
ENDIF(WIN32)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviident_MOC_SRCS ${kviident_MOC_HDRS})

SET(kvi_module_name kviident)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
