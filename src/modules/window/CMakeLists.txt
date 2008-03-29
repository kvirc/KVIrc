# CMakeLists for src/modules/window

SET(kvieventeditor_MOC_HDRS
	userwindow.h
)

SET(kviwindow_SRCS
	libkviwindow.cpp
	userwindow.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviwindow_MOC_SRCS ${kviwindow_MOC_HDRS})

# Create library
ADD_LIBRARY(kviwindow SHARED ${kviwindow_SRCS} ${kviwindow_MOC_SRCS})
#TARGET_LINK_LIBRARIES(kvilib kviwindow)

# Install target
SET_TARGET_PROPERTIES(kviwindow PROPERTIES VERSION ${KVI_VERSION} SOVERSION ${KVI_MAJOR})
INSTALL(TARGETS kviwindow LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)