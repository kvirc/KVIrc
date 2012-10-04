# - Find AUDIOFILE
# Find the native AUDIOFILE headers and libraries.
#
#  AUDIOFILE_INCLUDE_DIR -  where to find AUDIOFILE.h, etc.
#  AUDIOFILE_LIBRARIES    - List of libraries when using AUDIOFILE.
#  AUDIOFILE_FOUND        - True if AUDIOFILE found.

# Look for the header file.
find_path( AUDIOFILE_INCLUDE_DIR NAMES audiofile.h
		PATHS /usr/include /usr/local/include)
mark_as_advanced(AUDIOFILE_INCLUDE_DIR)

# Look for the library.
find_library( AUDIOFILE_LIBRARY NAMES audiofile
		PATHS /usr/lib /usr/lib64 )
mark_as_advanced(AUDIOFILE_LIBRARY)

# Copy the results to the output variables.
if(AUDIOFILE_INCLUDE_DIR AND AUDIOFILE_LIBRARY)
	set(AUDIOFILE_FOUND 1)
else()
	set(AUDIOFILE_FOUND 0)
endif()
