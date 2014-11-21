# CMakeLists for src/modules/rijndael

if(WANT_OPENSSL)
	subdirs(caps)

	set(kvirijndael_SRCS
		libkvirijndael.cpp
		Rijndael.cpp
		BlowFish.cpp
		UglyBase64.cpp
		InitVectorEngine.cpp
	)



	# set(ADDITIONAL_COMPILE_FLAGS "${ADDITIONAL_COMPILE_FLAGS} -fno-strict-aliasing")

	set(kvi_module_name kvirijndael)
	include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
endif()
