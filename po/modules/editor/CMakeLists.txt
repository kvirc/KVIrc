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

FILE(GLOB pofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.po")
FILE(GLOB mofiles "${CMAKE_CURRENT_SOURCE_DIR}/*.mo")

SET(POTFILE editor.pot)
SET(XGETTEXT_SOURCESDIR)
LIST(APPEND XGETTEXT_SOURCESDIR ${CMAKE_SOURCE_DIR}/src/modules/actioneditor)
LIST(APPEND XGETTEXT_SOURCESDIR ${CMAKE_SOURCE_DIR}/src/modules/aliaseditor)
LIST(APPEND XGETTEXT_SOURCESDIR ${CMAKE_SOURCE_DIR}/src/modules/classeditor)
LIST(APPEND XGETTEXT_SOURCESDIR ${CMAKE_SOURCE_DIR}/src/modules/codetester)
LIST(APPEND XGETTEXT_SOURCESDIR ${CMAKE_SOURCE_DIR}/src/modules/editor)
LIST(APPEND XGETTEXT_SOURCESDIR ${CMAKE_SOURCE_DIR}/src/modules/eventeditor)
LIST(APPEND XGETTEXT_SOURCESDIR ${CMAKE_SOURCE_DIR}/src/modules/popupeditor)
LIST(APPEND XGETTEXT_SOURCESDIR ${CMAKE_SOURCE_DIR}/src/modules/raweditor)
LIST(APPEND XGETTEXT_SOURCESDIR ${CMAKE_SOURCE_DIR}/src/modules/toolbareditor)

# Install target
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/translation.rules.txt)
