# CMakeLists for po/modules/register
# Includes:
# src/modules/regchan
# src/modules/reguser

FILE(GLOB pofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.po")
FILE(GLOB mofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.mo")

SET(POTFILE register.pot)
SET(XGETTEXT_SOURCESDIR)
LIST(APPEND XGETTEXT_SOURCESDIR ${CMAKE_SOURCE_DIR}/src/modules/regchan)
LIST(APPEND XGETTEXT_SOURCESDIR ${CMAKE_SOURCE_DIR}/src/modules/reguser)

# Install target
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/translation.rules.txt)
