# CMakeLists for data/config

FILE(GLOB files "*.kvc")
INSTALL(FILES ${files} DESTINATION ${KVI_INSTALL_PREFIX}/share/${KVI_PACKAGE}/${KVI_VERSION_BRANCH}/config/)
