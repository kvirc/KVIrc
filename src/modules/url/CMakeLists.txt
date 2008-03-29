# CMakeLists for src/modules/url

SET(kvieventeditor_MOC_HDRS
	libkviurl.h
	icons.h
)

SET(kviurl_SRCS
	libkviurl.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviurl_MOC_SRCS ${kviurl_MOC_HDRS})

# Create library
ADD_LIBRARY(kviurl SHARED ${kviurl_SRCS} ${kviurl_MOC_SRCS})
#TARGET_LINK_LIBRARIES(kvilib kviurl)

# Install target
SET_TARGET_PROPERTIES(kviurl PROPERTIES VERSION ${KVI_VERSION} SOVERSION ${KVI_MAJOR})
INSTALL(TARGETS kviurl LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)