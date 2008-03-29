# CMakeLists for src/modules/help

SET(kvihelp_MOC_HDRS
	helpwidget.h
	helpwindow.h
	index.h
)

SET(kvihelp_SRCS
	libkvihelp.cpp
	helpwidget.cpp
	helpwindow.cpp
	index.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvihelp_MOC_SRCS ${kvihelp_MOC_HDRS})

# Create library
ADD_LIBRARY(kvihelp SHARED ${kvihelp_SRCS} ${kvihelp_MOC_SRCS})
#TARGET_LINK_LIBRARIES(kvilib kvihelp)

# Install target
SET_TARGET_PROPERTIES(kvihelp PROPERTIES VERSION ${KVI_VERSION} SOVERSION ${KVI_MAJOR})
INSTALL(TARGETS kvihelp LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)