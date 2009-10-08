# CMakeLists for src/modules/str

IF(WANT_NO_EMBEDDED_CODE)
    ADD_DEFINITIONS("-DCOMPILE_NO_EMBEDDED_CODE -fexceptions")
ELSEIF(WANT_NO_EMBEDDED_CODE)
    IF(UNIX)
	    IF(APPLE)
		    LIST(APPEND LIBS -lssl -lcrypto)
	    ENDIF()
    ENDIF()
ENDIF(WANT_NO_EMBEDDED_CODE)

SET(kvistr_SRCS
	libkvistr.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvistr_MOC_SRCS ${kvistr_MOC_HDRS})

SET(kvi_module_name kvistr)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
