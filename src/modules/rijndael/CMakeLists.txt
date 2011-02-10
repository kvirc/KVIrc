# CMakeLists for src/modules/rijndael
IF(WANT_CRYPTOPP OR WANT_OPENSSL)

	SUBDIRS(caps)

	IF(WANT_CRYPTOPP)
		SET(kvirijndael_SRCS 
			libkvirijndael_cryptopp.cpp
			UglyBase64.cpp
		)

		SET(kvirijndael_MOC_HDRS
			libkvirijndael_cryptopp.h
		)
	ELSE(WANT_CRYPTOPP)
		SET(kvirijndael_SRCS
			libkvirijndael.cpp
			Rijndael.cpp
			BlowFish.cpp
			UglyBase64.cpp
		)

		SET(kvirijndael_MOC_HDRS
			libkvirijndael.h
		)
	ENDIF(WANT_CRYPTOPP)

	# After this call, files will be moc'ed to moc_kvi_*.cpp
	QT4_WRAP_CPP(kvirijndael_MOC_SRCS ${kvirijndael_MOC_HDRS})

	SET(ADDITIONAL_COMPILE_FLAGS "${ADDITIONAL_COMPILE_FLAGS} -fno-strict-aliasing")

	SET(kvi_module_name kvirijndael)
	INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
ENDIF(WANT_CRYPTOPP OR WANT_OPENSSL)
