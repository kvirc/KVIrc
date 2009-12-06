# Taken from Kde4 Multimedia source files. Modified to use the new theora 1.1 api
# by Fabio Bas (ctrlaltca at libero dot it).
# You can find a copy of the mentioned COPYING-CMAKE-SCRIPTS file here:
# http://websvn.kde.org/trunk/KDE/kdemultimedia/cmake/modules/
# Original copyright header follows:

# Option for build or not Theora

# Copyright (c) 2006,2007 Laurent Montel, <montel@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


IF(THEORA_INCLUDE_DIR AND THEORA_LIB_LIBRARIES_ENC AND THEORA_LIB_LIBRARIES_DEC AND THEORA_VORBIS_LIBRARIES AND THEORA_OGG_LIBRARIES)
	# Already in cache, be silent
	set(Theora_FIND_QUIETLY TRUE)	
ENDIF()

FIND_PATH(THEORA_INCLUDE_DIR theora/theoraenc.h)
FIND_LIBRARY(THEORA_OGG_LIBRARIES NAMES ogg )
FIND_LIBRARY(THEORA_VORBIS_LIBRARIES NAMES vorbis)
FIND_LIBRARY(THEORA_VORBIS_LIBRARIES_ENC NAMES vorbisenc)
FIND_LIBRARY(THEORA_LIB_LIBRARIES_ENC NAMES theoraenc)
FIND_LIBRARY(THEORA_LIB_LIBRARIES_DEC NAMES theoradec)

IF(THEORA_VORBIS_LIBRARIES AND THEORA_VORBIS_LIBRARIES_ENC AND THEORA_LIB_LIBRARIES_ENC AND THEORA_LIB_LIBRARIES_DEC AND THEORA_INCLUDE_DIR)	
	set(THEORA_LIBRARY ${THEORA_LIB_LIBRARIES_ENC} ${THEORA_LIB_LIBRARIES_DEC} ${THEORA_OGG_LIBRARIES} ${THEORA_VORBIS_LIBRARIES} ${THEORA_VORBIS_LIBRARIES_ENC})
	set(THEORA_FOUND TRUE)
ENDIF()

IF(THEORA_FOUND)
	IF(NOT Theora_FIND_QUIETLY)
		MESSAGE(STATUS "Found theora: includes in ${THEORA_INCLUDE_DIR}, library in ${THEORA_LIBRARY}")
	ENDIF()
ELSE()
	IF(Theora_FIND_REQUIRED)
		MESSAGE(STATUS "Theora not found")
	ENDIF()
ENDIF()

MARK_AS_ADVANCED(THEORA_INCLUDE_DIR THEORA_LIBRARY)
