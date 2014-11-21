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


if(THEORA_INCLUDE_DIR AND THEORA_LIB_LIBRARIES_ENC AND THEORA_LIB_LIBRARIES_DEC AND THEORA_VORBIS_LIBRARIES AND THEORA_OGG_LIBRARIES)
	# Already in cache, be silent
	set(Theora_FIND_QUIETLY TRUE)
endif()

find_path(THEORA_INCLUDE_DIR theora/theoraenc.h)
find_library(THEORA_OGG_LIBRARIES NAMES ogg )
find_library(THEORA_VORBIS_LIBRARIES NAMES vorbis)
find_library(THEORA_VORBIS_LIBRARIES_ENC NAMES vorbisenc)
find_library(THEORA_LIB_LIBRARIES_ENC NAMES theoraenc)
find_library(THEORA_LIB_LIBRARIES_DEC NAMES theoradec)

if(THEORA_VORBIS_LIBRARIES AND THEORA_VORBIS_LIBRARIES_ENC AND THEORA_LIB_LIBRARIES_ENC AND THEORA_LIB_LIBRARIES_DEC AND THEORA_INCLUDE_DIR)
	set(THEORA_LIBRARY ${THEORA_LIB_LIBRARIES_ENC} ${THEORA_LIB_LIBRARIES_DEC} ${THEORA_OGG_LIBRARIES} ${THEORA_VORBIS_LIBRARIES} ${THEORA_VORBIS_LIBRARIES_ENC})
	set(THEORA_FOUND TRUE)
endif()

if(THEORA_FOUND)
	if(NOT Theora_FIND_QUIETLY)
		message(STATUS "Found theora: includes in ${THEORA_INCLUDE_DIR}, library in ${THEORA_LIBRARY}")
	endif()
else()
	if(Theora_FIND_REQUIRED)
		message(STATUS "Theora not found")
	endif()
endif()

mark_as_advanced(THEORA_INCLUDE_DIR THEORA_LIBRARY)
