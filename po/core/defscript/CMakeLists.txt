# CMakeLists for po/core/defscript/

# sed will cut paths at this directory
set(TRANSLATION_BASEDIR "data")

file(GLOB pofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.po")
file(GLOB mofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.mo")

set(POTFILE defscript.pot)
set(XGETTEXT_SOURCESDIR)
list(APPEND XGETTEXT_SOURCESDIR ${CMAKE_SOURCE_DIR}/data/defscript)

# this enables specific xgettext parameters for parsing .kvs scripts
set(TRANSLATION_DEFSCRIPT 1)

# Install target
include(${CMAKE_SOURCE_DIR}/cmake/translation.rules.txt)
