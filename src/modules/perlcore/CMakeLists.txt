# CMakeLists for src/modules/perlcore

set(kviperlcore_SRCS
	libkviperlcore.cpp
)

set(kvi_module_name kviperlcore)

if(COMPILE_PERL_SUPPORT)
	# Get compile flags
	execute_process(COMMAND ${PERL_EXECUTABLE} -MExtUtils::Embed -e ccopts
		OUTPUT_VARIABLE PERL_COMPILE_FLAGS
	)

	if(CMAKE_COMPILER_IS_GNUCXX)
		# GCC whines because perl isn't extremely C++11 compliant
		set(PERL_COMPILE_FLAGS "${PERL_COMPILE_FLAGS} -Wno-literal-suffix")
	elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
		# If the perl version is too old, clang will throw errors due to C++11
		# incompatibilities
		set(PERL_COMPILE_FLAGS "${PERL_COMPILE_FLAGS} -Wno-reserved-user-defined-literal")
	endif()

	#remove new line characters
	string(REGEX REPLACE "(\r?\n)+$" "" PERL_COMPILE_FLAGS ${PERL_COMPILE_FLAGS})
	#remove leading and trailing spaces (string strip)
	string(REGEX REPLACE "(^( )+|( )+$)" "" PERL_COMPILE_FLAGS ${PERL_COMPILE_FLAGS})

	# Get links flags
	execute_process(
		COMMAND ${PERL_EXECUTABLE} -MExtUtils::Embed -e ldopts
		OUTPUT_VARIABLE PERL_LINK_FLAGS
	)

	if(MSVC)
		# breaks with -nodefaultlib
		string(REPLACE " -nodefaultlib" "" PERL_LINK_FLAGS "${PERL_LINK_FLAGS}")
		# conflicts with opt level set by user, especially in debug mode
		string(REPLACE " -O2" "" PERL_COMPILE_FLAGS "${PERL_COMPILE_FLAGS}")
	endif()

	if(APPLE)
		# ldopts contains all archs supported by the current platform, but we
		# only want those we specified in CMAKE_OSX_ARCHITECTURES.
		string(REGEX REPLACE "-arch [-a-zA-Z0-9_]*" "" PERL_LINK_FLAGS "${PERL_LINK_FLAGS}")
		foreach (arch ${CMAKE_OSX_ARCHITECTURES})
			set(PERL_LINK_FLAGS " -arch ${arch} ${PERL_LINK_FLAGS}")
		endforeach(arch)
	endif()

	# Remove new line characters
	string(REGEX REPLACE "(\r?\n)+$" "" PERL_LINK_FLAGS "${PERL_LINK_FLAGS}")
	# Remove leading and trailing spaces (string strip)
	string(REGEX REPLACE "(^( )+|( )+$)" "" PERL_LINK_FLAGS "${PERL_LINK_FLAGS}")
endif()

include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)

if(COMPILE_PERL_SUPPORT)
	set_target_properties(${kvi_module_name} PROPERTIES COMPILE_FLAGS ${PERL_COMPILE_FLAGS})
	set_target_properties(${kvi_module_name} PROPERTIES LINK_FLAGS "${MODULE_ADDITIONAL_LINK_FLAGS}")
	target_link_libraries(${kvi_module_name} ${LIBS} "${PERL_LINK_FLAGS}")
endif()
