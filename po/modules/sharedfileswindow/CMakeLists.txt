# CMakeLists for po/modules/sharedfileswindow

# Includes:
# src/modules/sharedfile
# src/modules/sharedfileswindow


file(GLOB pofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.po")
file(GLOB mofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.mo")

set(POTFILE sharedfileswindow.pot)
set(XGETTEXT_SOURCESDIR)
list(APPEND XGETTEXT_SOURCESDIR ${CMAKE_SOURCE_DIR}/src/modules/sharedfile)
list(APPEND XGETTEXT_SOURCESDIR ${CMAKE_SOURCE_DIR}/src/modules/sharedfileswindow)

# Install target
include(${CMAKE_SOURCE_DIR}/cmake/translation.rules.txt)
