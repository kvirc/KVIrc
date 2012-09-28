# CMakeLists for src/modules/rijndael

IF(WANT_OPENSSL)
	SUBDIRS(caps)

	SET(kvirijndael_SRCS
		libkvirijndael.cpp
		Rijndael.cpp
		BlowFish.cpp
		UglyBase64.cpp
		InitVectorEngine.cpp
	)

	

	# SET(ADDITIONAL_COMPILE_FLAGS "${ADDITIONAL_COMPILE_FLAGS} -fno-strict-aliasing")

	SET(kvi_module_name kvirijndael)
	INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
ENDIF()
