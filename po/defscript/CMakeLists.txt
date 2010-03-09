# CMakeLists for po/defscript

FILE(GLOB pofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.po")
FILE(GLOB mofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.mo")

SET(POTFILE defscript.pot)
SET(XGETTEXT_SOURCESDIR)
LIST(APPEND XGETTEXT_SOURCESDIR ${CMAKE_SOURCE_DIR}/data/defscript)
#THIS IS SPARTA!
SET(TRANSLATION_DEFSCRIPT 1) # Hm ?

# Install target
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/translation.rules.txt)
