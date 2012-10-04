# CMakeLists for po/modules/python

# Includes:
# src/modules/python
# src/modules/pythoncore

file(GLOB pofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.po")
file(GLOB mofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.mo")

set(POTFILE python.pot)
set(XGETTEXT_SOURCESDIR)
list(APPEND XGETTEXT_SOURCESDIR ${CMAKE_SOURCE_DIR}/src/modules/python)
list(APPEND XGETTEXT_SOURCESDIR ${CMAKE_SOURCE_DIR}/src/modules/pythoncore)

# Install target
include(${CMAKE_SOURCE_DIR}/cmake/translation.rules.txt)
