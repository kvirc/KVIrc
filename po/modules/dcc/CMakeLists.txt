# CMakeLists for po/modules/dcc

file(GLOB pofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.po")
file(GLOB mofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.mo")

set(POTFILE dcc.pot)
set(XGETTEXT_SOURCESDIR ${CMAKE_SOURCE_DIR}/src/modules/dcc)

# Install target
include(${CMAKE_SOURCE_DIR}/cmake/translation.rules.txt)
