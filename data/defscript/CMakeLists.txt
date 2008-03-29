# CMakeLists for data/defscript

FILE(GLOB files "*.kvs")
INSTALL(FILES ${files} DESTINATION ${KVI_INSTALL_PREFIX}/share/${KVI_PACKAGE}/${KVI_VERSION_BRANCH}/defscript/)