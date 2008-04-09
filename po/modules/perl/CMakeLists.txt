# CMakeLists for po/modules/perl

FILE(GLOB pofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.po")
FILE(GLOB mofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.mo")

# Install target
GETTEXT_CREATE_TRANSLATIONS(perl.pot ALL ${pofiles})
