# CMakeLists for po/modules/torrent

file(GLOB pofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.po")
file(GLOB mofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.mo")

set(POTFILE torrent.pot)
set(XGETTEXT_SOURCESDIR ${CMAKE_SOURCE_DIR}/src/modules/torrent)

# Install target
include(${CMAKE_SOURCE_DIR}/cmake/translation.rules.txt)
