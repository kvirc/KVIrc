# CMakeLists for src/modules/links

SET(kvilinks_MOC_HDRS
	linkswindow.h
)

SET(kvilinks_SRCS
	libkvilinks.cpp
	linkswindow.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvilinks_MOC_SRCS ${kvilinks_MOC_HDRS})

# Create library
ADD_LIBRARY(kvilinks SHARED ${kvilinks_SRCS} ${kvilinks_MOC_SRCS})
#TARGET_LINK_LIBRARIES(kvilib kvilinks)

# Install target
SET_TARGET_PROPERTIES(kvilinks PROPERTIES VERSION ${KVI_VERSION} SOVERSION ${KVI_MAJOR})
INSTALL(TARGETS kvilinks LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)