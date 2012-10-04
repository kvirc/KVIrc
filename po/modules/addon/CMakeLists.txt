# CMakeLists for po/modules/addon

file(GLOB pofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.po")
file(GLOB mofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.mo")

set(POTFILE addon.pot)
set(XGETTEXT_SOURCESDIR ${CMAKE_SOURCE_DIR}/src/modules/addon)

# Install target
include(${CMAKE_SOURCE_DIR}/cmake/translation.rules.txt)
