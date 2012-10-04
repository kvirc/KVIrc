# CMakeLists for src/modules/perlcore

set(kviperlcore_SRCS
	libkviperlcore.cpp
)

set(kvi_module_name kviperlcore)

if(COMPILE_PERL_SUPPORT)
	execute_process(COMMAND ${PERL_EXECUTABLE} -MExtUtils::Embed -e ccopts
		OUTPUT_VARIABLE PERL_COMPILE_FLAGS
	)

	#remove new line characters
	string(REGEX REPLACE "(\r?\n)+$" "" PERL_COMPILE_FLAGS ${PERL_COMPILE_FLAGS})
	#remove leading and trailing spaces (string strip)
	string(REGEX REPLACE "(^( )+|( )+$)" "" PERL_COMPILE_FLAGS ${PERL_COMPILE_FLAGS})
endif()

include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)

if(COMPILE_PERL_SUPPORT)
	if(WANT_DEBUG AND MSVC)
		# Activestates Perl does not support Run-Time Error checks
		string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG_INIT ${CMAKE_CXX_FLAGS_DEBUG_INIT})
		string(REPLACE "/RTC1" "" CMAKE_C_FLAGS_DEBUG_INIT ${CMAKE_C_FLAGS_DEBUG_INIT})
		string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
		string(REPLACE "/RTC1" "" CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})
	endif()
	set_target_properties(${kvi_module_name} PROPERTIES COMPILE_FLAGS ${PERL_COMPILE_FLAGS})
endif()
