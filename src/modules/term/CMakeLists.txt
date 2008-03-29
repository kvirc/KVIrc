# CMakeLists for src/modules/term

SET(kvieventeditor_MOC_HDRS
	termwidget.h
	termwindow.h
)

SET(kviterm_SRCS
	libkviterm.cpp
	termwidget.cpp
	termwindow.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviterm_MOC_SRCS ${kviterm_MOC_HDRS})

# Create library
ADD_LIBRARY(kviterm SHARED ${kviterm_SRCS} ${kviterm_MOC_SRCS})
#TARGET_LINK_LIBRARIES(kvilib kviterm)

# Install target
SET_TARGET_PROPERTIES(kviterm PROPERTIES VERSION ${KVI_VERSION} SOVERSION ${KVI_MAJOR})
INSTALL(TARGETS kviterm LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)