# CMakeLists for po/modules/logview
# Includes:
# src/modules/log
# src/modules/logview

FILE(GLOB pofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.po")
FILE(GLOB mofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.mo")

SET(POTFILE log.pot)
SET(XGETTEXT_SOURCESDIR)
LIST(APPEND XGETTEXT_SOURCESDIR ${CMAKE_SOURCE_DIR}/src/modules/log)
LIST(APPEND XGETTEXT_SOURCESDIR ${CMAKE_SOURCE_DIR}/src/modules/logview)

# Install target
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/translation.rules.txt)
