# CMakeLists for po/modules/notifier

file(GLOB pofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.po")
file(GLOB mofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.mo")

set(POTFILE notifier.pot)
set(XGETTEXT_SOURCESDIR ${CMAKE_SOURCE_DIR}/src/modules/notifier)

# Install target
include(${CMAKE_SOURCE_DIR}/cmake/translation.rules.txt)
