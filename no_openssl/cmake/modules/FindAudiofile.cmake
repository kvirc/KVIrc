# - Find AUDIOFILE
# Find the native AUDIOFILE headers and libraries.
#
#  AUDIOFILE_INCLUDE_DIR -  where to find AUDIOFILE.h, etc.
#  AUDIOFILE_LIBRARIES    - List of libraries when using AUDIOFILE.
#  AUDIOFILE_FOUND        - True if AUDIOFILE found.

# Look for the header file.
FIND_PATH( AUDIOFILE_INCLUDE_DIR NAMES audiofile.h
		PATHS /usr/include /usr/local/include)
MARK_AS_ADVANCED(AUDIOFILE_INCLUDE_DIR)

# Look for the library.
FIND_LIBRARY( AUDIOFILE_LIBRARY NAMES audiofile
		PATHS /usr/lib /usr/lib64 )
MARK_AS_ADVANCED(AUDIOFILE_LIBRARY)

# Copy the results to the output variables.
IF(AUDIOFILE_INCLUDE_DIR AND AUDIOFILE_LIBRARY)
	SET(AUDIOFILE_FOUND 1)
ELSE()
	SET(AUDIOFILE_FOUND 0)
ENDIF()