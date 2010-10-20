# CMakeLists for po/core/kvirc/

# sed will cut paths at this directory
SET(TRANSLATION_BASEDIR "src")

FILE(GLOB pofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.po")
FILE(GLOB mofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.mo")

SET(POTFILE kvirc.pot)
SET(XGETTEXT_SOURCESDIR)
LIST(APPEND XGETTEXT_SOURCESDIR ${CMAKE_SOURCE_DIR}/src)
#THIS IS SPARTA!
SET(TRANSLATION_KVIRC_CORE 1)

# Install target
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/translation.rules.txt)
