# CMakeLists for po/modules/perlcore

FILE(GLOB pofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.po")
FILE(GLOB mofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.mo")

# Install target
GETTEXT_CREATE_TRANSLATIONS(perlcore.pot ALL ${pofiles})
