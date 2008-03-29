# CMakeLists for src/modules/list

SET(kvilist_MOC_HDRS
	listwindow.h
)

SET(kvilist_SRCS
	libkvilist.cpp
	listwindow.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvilist_MOC_SRCS ${kvilist_MOC_HDRS})

# Create library
ADD_LIBRARY(kvilist SHARED ${kvilist_SRCS} ${kvilist_MOC_SRCS})
#TARGET_LINK_LIBRARIES(kvilib kvilist)

# Install target
SET_TARGET_PROPERTIES(kvilist PROPERTIES VERSION ${KVI_VERSION} SOVERSION ${KVI_MAJOR})
INSTALL(TARGETS kvilist LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)