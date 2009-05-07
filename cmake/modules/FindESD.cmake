# - Find ESD
# Find ESD headers and libraries.
#
#  ESD_INCLUDE_DIR  -  where to find esd.h, etc.
#  ESD_LIBRARY      - List of libraries when using ESD.
#  ESD_FOUND        - True if ESD found.

# Look for the header file.
FIND_PATH( ESD_INCLUDE_DIR NAMES esd.h
           PATHS /usr/include /usr/local/include)
MARK_AS_ADVANCED(ESD_INCLUDE_DIR)

# Look for the library.
FIND_LIBRARY( ESD_LIBRARY NAMES esd
              PATHS /usr/lib /usr/lib64 )
MARK_AS_ADVANCED(ESD_LIBRARY)

# Copy the results to the output variables.
IF(ESD_INCLUDE_DIR AND ESD_LIBRARY)
  SET(ESD_FOUND 1)
ELSE(ESD_INCLUDE_DIR AND ESD_LIBRARY)
  SET(ESD_FOUND 0)
ENDIF(ESD_INCLUDE_DIR AND ESD_LIBRARY)