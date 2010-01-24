# CMakeLists for po/modules/sharedfile

FILE(GLOB pofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.po")
FILE(GLOB mofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.mo")

SET(POTFILE sharedfile.pot)
SET(XGETTEXT_SOURCESDIR ${CMAKE_SOURCE_DIR}/src/modules/sharedfile)

# Install target
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/translation.rules.txt)
