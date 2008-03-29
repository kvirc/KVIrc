# CMakeLists for data/pics/coresmall

FILE(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.png")
INSTALL(FILES ${files} DESTINATION ${KVI_INSTALL_PREFIX}/share/${KVI_PACKAGE}/${KVI_VERSION_BRANCH}/pics/coresmall)