# CMakeLists for po/core/kvirc/

# sed will cut paths at this directory
set(TRANSLATION_BASEDIR "src")

file(GLOB pofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.po")
file(GLOB mofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.mo")

set(POTFILE kvirc.pot)
set(XGETTEXT_SOURCESDIR)
list(APPEND XGETTEXT_SOURCESDIR ${CMAKE_SOURCE_DIR}/src)
#THIS IS SPARTA!
set(TRANSLATION_KVIRC_CORE 1)

# Install target
include(${CMAKE_SOURCE_DIR}/cmake/translation.rules.txt)
