# CMakeLists for po/modules/serverdb

# Includes:
# src/modules/proxydb
# src/modules/serverdb

file(GLOB pofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.po")
file(GLOB mofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.mo")

set(POTFILE serverdb.pot)
set(XGETTEXT_SOURCESDIR)
list(APPEND XGETTEXT_SOURCESDIR ${CMAKE_SOURCE_DIR}/src/modules/proxydb)
list(APPEND XGETTEXT_SOURCESDIR ${CMAKE_SOURCE_DIR}/src/modules/serverdb)

# Install target
include(${CMAKE_SOURCE_DIR}/cmake/translation.rules.txt)
