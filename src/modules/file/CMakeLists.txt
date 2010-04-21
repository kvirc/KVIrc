# CMakeLists for src/modules/file

IF(WANT_NO_EMBEDDED_CODE)
    ADD_DEFINITIONS("-DCOMPILE_NO_EMBEDDED_CODE -fexceptions")
ENDIF(WANT_NO_EMBEDDED_CODE)

SET(kvifile_SRCS
	libkvifile.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvifile_MOC_SRCS ${kvifile_MOC_HDRS})

SET(kvi_module_name kvifile)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
