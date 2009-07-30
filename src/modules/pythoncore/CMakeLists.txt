# CMakeLists for src/modules/pythoncore
#
# Only build this if WITH_PYTHON was passed to CMake.

IF(WANT_PYTHON)
    SET(kvipythoncore_MOC_HDRS
	    pythoncoreinterface.h
    )

    SET(kvipythoncore_SRCS
	    libkvipythoncore.cpp
	    kvircmodule.cpp
    )

    # After this call, files will be moc'ed to moc_kvi_*.cpp
    QT4_WRAP_CPP(kvipythoncore_MOC_SRCS ${kvipythoncore_MOC_HDRS})

    SET(kvi_module_name kvipythoncore)

    INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
ENDIF(WANT_PYTHON)
