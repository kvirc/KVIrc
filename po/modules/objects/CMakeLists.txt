# CMakeLists for po/modules/objects

FILE(GLOB pofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.po")
FILE(GLOB mofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.mo")

SET(POTFILE objects.pot)
SET(XGETTEXT_SOURCESDIR ${CMAKE_SOURCE_DIR}/src/modules/objects)

# Install target
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/translation.rules.txt)
