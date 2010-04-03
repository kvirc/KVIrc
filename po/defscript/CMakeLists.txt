# CMakeLists for po/defscript

# sed will cut paths at this directory
SET(TRANSLATION_BASEDIR "data")

FILE(GLOB pofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.po")
FILE(GLOB mofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.mo")

SET(POTFILE defscript.pot)
SET(XGETTEXT_SOURCESDIR)
LIST(APPEND XGETTEXT_SOURCESDIR ${CMAKE_SOURCE_DIR}/data/defscript)

# this enables specific xgettext parameters for parsing .kvs scripts
SET(TRANSLATION_DEFSCRIPT 1)

# Install target
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/translation.rules.txt)
