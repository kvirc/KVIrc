# CMakeLists for src/modules/dialog

SET(kvidialog_MOC_HDRS
	libkvidialog.h
)

SET(kvidialog_SRCS
	libkvidialog.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvidialog_MOC_SRCS ${kvidialog_MOC_HDRS})

# Create library
ADD_LIBRARY(kvidialog SHARED ${kvidialog_SRCS} ${kvidialog_MOC_SRCS})
#TARGET_LINK_LIBRARIES(kvilib kvidialog)

# Install target
SET_TARGET_PROPERTIES(kvidialog PROPERTIES VERSION ${KVI_VERSION} SOVERSION ${KVI_MAJOR})
INSTALL(TARGETS kvidialog LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)