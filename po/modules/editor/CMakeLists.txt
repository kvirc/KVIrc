# CMakeLists for po/modules/editor

# Includes:
# src/modules/actioneditor
# src/modules/aliaseditor
# src/modules/classeditor
# src/modules/editor
# src/modules/eventeditor
# src/modules/popupeditor
# src/modules/raweditor
# src/modules/toolbareditor

file(GLOB pofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.po")
file(GLOB mofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.mo")

set(POTFILE editor.pot)
set(XGETTEXT_SOURCESDIR)
list(APPEND XGETTEXT_SOURCESDIR ${CMAKE_SOURCE_DIR}/src/modules/actioneditor)
list(APPEND XGETTEXT_SOURCESDIR ${CMAKE_SOURCE_DIR}/src/modules/aliaseditor)
list(APPEND XGETTEXT_SOURCESDIR ${CMAKE_SOURCE_DIR}/src/modules/classeditor)
list(APPEND XGETTEXT_SOURCESDIR ${CMAKE_SOURCE_DIR}/src/modules/codetester)
list(APPEND XGETTEXT_SOURCESDIR ${CMAKE_SOURCE_DIR}/src/modules/editor)
list(APPEND XGETTEXT_SOURCESDIR ${CMAKE_SOURCE_DIR}/src/modules/eventeditor)
list(APPEND XGETTEXT_SOURCESDIR ${CMAKE_SOURCE_DIR}/src/modules/popupeditor)
list(APPEND XGETTEXT_SOURCESDIR ${CMAKE_SOURCE_DIR}/src/modules/raweditor)
list(APPEND XGETTEXT_SOURCESDIR ${CMAKE_SOURCE_DIR}/src/modules/toolbareditor)

# Install target
include(${CMAKE_SOURCE_DIR}/cmake/translation.rules.txt)
