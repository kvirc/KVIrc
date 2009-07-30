# CMakeLists for src/modules/python
#
# Only build this, if WITH_PYTHON was set and passed to CMake

IF(COMPILE_PYTHON_SUPPORT)
    SET(kvipython_SRCS
	    libkvipython.cpp
    )

    # After this call, files will be moc'ed to moc_kvi_*.cpp
    QT4_WRAP_CPP(kvipython_MOC_SRCS ${kvipython_MOC_HDRS})

    SET(kvi_module_name kvipython)

    INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
ENDIF(COMPILE_PYTHON_SUPPORT)
