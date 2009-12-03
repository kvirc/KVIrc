# CMakeLists for src/modules/rijndael
IF(WANT_NO_EMBEDDED_CODE OR WANT_OPENSSL)

SUBDIRS(caps)

IF(WANT_NO_EMBEDDED_CODE)
    ADD_DEFINITIONS("-fexceptions")

    SET(kvirijndael_SRCS 
            libkvirijndael_cryptopp.cpp
       )

    SET(kvirijndael_MOC_HDRS
            libkvirijndael_cryptopp.h
       )
ELSE(WANT_NO_EMBEDDED_CODE)
    SET(kvirijndael_SRCS
	    libkvirijndael.cpp
	    rijndael.cpp
	    ablowfish.cpp
    )

    SET(kvirijndael_MOC_HDRS
	    libkvirijndael.h
    )
ENDIF(WANT_NO_EMBEDDED_CODE)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvirijndael_MOC_SRCS ${kvirijndael_MOC_HDRS})

SET(ADDITIONAL_LINK_FLAGS "${ADDITIONAL_LINK_FLAGS} -fno-strict-aliasing")

SET(kvi_module_name kvirijndael)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
ENDIF(WANT_NO_EMBEDDED_CODE OR WANT_OPENSSL)
