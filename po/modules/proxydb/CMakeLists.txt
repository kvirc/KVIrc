# CMakeLists for po/modules/proxydb

FILE(GLOB pofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.po")
FILE(GLOB mofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.mo")

SET(POTFILE proxydb.pot)
SET(XGETTEXT_SOURCESDIR ${CMAKE_SOURCE_DIR}/src/modules/proxydb)

# Install target
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/translation.rules.txt)
