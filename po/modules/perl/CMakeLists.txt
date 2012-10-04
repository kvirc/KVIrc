# CMakeLists for po/modules/perl

# Includes:
# src/modules/perl
# src/modules/perlcore

file(GLOB pofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.po")
file(GLOB mofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.mo")

set(POTFILE perl.pot)
set(XGETTEXT_SOURCESDIR)
list(APPEND XGETTEXT_SOURCESDIR ${CMAKE_SOURCE_DIR}/src/modules/perl)
list(APPEND XGETTEXT_SOURCESDIR ${CMAKE_SOURCE_DIR}/src/modules/perlcore)

# Install target
include(${CMAKE_SOURCE_DIR}/cmake/translation.rules.txt)
