# CMakeLists for data/msgcolors

FILE(GLOB files "*.msgclr")
INSTALL(FILES ${files} DESTINATION ${KVI_INSTALL_PREFIX}/share/${KVI_PACKAGE}/${KVI_VERSION_BRANCH}/msgcolors/)