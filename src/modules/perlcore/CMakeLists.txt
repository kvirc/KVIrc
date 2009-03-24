# CMakeLists for src/modules/perlcore

SET(kviperlcore_SRCS
	libkviperlcore.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviperlcore_MOC_SRCS ${kviperlcore_MOC_HDRS})

SET(kvi_module_name kviperlcore)

IF(COMPILE_PERL_SUPPORT)
	EXECUTE_PROCESS(COMMAND ${PERL_EXECUTABLE} -MExtUtils::Embed -e ccopts
		OUTPUT_VARIABLE PERL_COMPILE_FLAGS
	)

	#remove new line characters
	STRING(REGEX REPLACE "(\r?\n)+$" "" PERL_COMPILE_FLAGS ${PERL_COMPILE_FLAGS})
	#remove leading and trailing spaces (string strip)
	STRING(REGEX REPLACE "(^( )+|( )+$)" "" PERL_COMPILE_FLAGS ${PERL_COMPILE_FLAGS})
ENDIF()

INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)

IF(COMPILE_PERL_SUPPORT)
	SET_TARGET_PROPERTIES(${kvi_module_name} PROPERTIES COMPILE_FLAGS ${PERL_COMPILE_FLAGS})
ENDIF()