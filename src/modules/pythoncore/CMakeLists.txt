# CMakeLists for src/modules/pythoncore

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

IF(COMPILE_PYTHON_SUPPORT)
	EXECUTE_PROCESS(COMMAND ${PYTHON_EXECUTABLE}-config --cflags
		OUTPUT_VARIABLE PYTHON_COMPILE_FLAGS
	)

	# Remove new line characters
	STRING(REGEX REPLACE "(\r?\n)+$" "" PYTHON_COMPILE_FLAGS "${PYTHON_COMPILE_FLAGS}")
	# Remove leading and trailing spaces (string strip)
	STRING(REGEX REPLACE "(^( )+|( )+$)" "" PYTHON_COMPILE_FLAGS "${PYTHON_COMPILE_FLAGS}")
ENDIF()

INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)

IF(COMPILE_PYTHON_SUPPORT)
	SET_TARGET_PROPERTIES(${kvi_module_name} PROPERTIES COMPILE_FLAGS "${PYTHON_COMPILE_FLAGS}")
ENDIF()
