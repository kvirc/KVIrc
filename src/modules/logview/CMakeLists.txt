# CMakeLists for src/modules/logview

SET(kvilogview_MOC_HDRS
	logviewwidget.h
	logviewmdiwindow.h
)

SET(kvilogview_SRCS
	libkvilogview.cpp
	logviewwidget.cpp
	logviewmdiwindow.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvilogview_MOC_SRCS ${kvilogview_MOC_HDRS})

# Create library
ADD_LIBRARY(kvilogview SHARED ${kvilogview_SRCS} ${kvilogview_MOC_SRCS})
#TARGET_LINK_LIBRARIES(kvilib kvilogview)

# Install target
SET_TARGET_PROPERTIES(kvilogview PROPERTIES VERSION ${KVI_VERSION} SOVERSION ${KVI_MAJOR})
INSTALL(TARGETS kvilogview LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)