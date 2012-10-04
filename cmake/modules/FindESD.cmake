# - Find ESD
# Find ESD headers and libraries.
#
#  ESD_INCLUDE_DIR  -  where to find esd.h, etc.
#  ESD_LIBRARY      - List of libraries when using ESD.
#  ESD_FOUND        - True if ESD found.

# Look for the header file.
find_path( ESD_INCLUDE_DIR NAMES esd.h
           PATHS /usr/include /usr/local/include)
mark_as_advanced(ESD_INCLUDE_DIR)

# Look for the library.
find_library( ESD_LIBRARY NAMES esd
              PATHS /usr/lib /usr/lib64 )
mark_as_advanced(ESD_LIBRARY)

# Copy the results to the output variables.
if(ESD_INCLUDE_DIR AND ESD_LIBRARY)
  set(ESD_FOUND 1)
else(ESD_INCLUDE_DIR AND ESD_LIBRARY)
  set(ESD_FOUND 0)
endif(ESD_INCLUDE_DIR AND ESD_LIBRARY)
