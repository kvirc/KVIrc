# CMakeLists for po/modules/python
# Includes:
# src/modules/python
# src/modules/pythoncore

FILE(GLOB pofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.po")
FILE(GLOB mofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.mo")

SET(POTFILE python.pot)
SET(XGETTEXT_SOURCESDIR)
LIST(APPEND XGETTEXT_SOURCESDIR ${CMAKE_SOURCE_DIR}/src/modules/python)
LIST(APPEND XGETTEXT_SOURCESDIR ${CMAKE_SOURCE_DIR}/src/modules/pythoncore)

# Install target
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/translation.rules.txt)
