# CMakeLists for src/modules/log

SET(kvilog_SRCS
	libkvilog.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvilog_MOC_SRCS ${kvilog_MOC_HDRS})

# Create library
ADD_LIBRARY(kvilog SHARED ${kvilog_SRCS} ${kvilog_MOC_SRCS})
#TARGET_LINK_LIBRARIES(kvilib kvilog)

# Install target
SET_TARGET_PROPERTIES(kvilog PROPERTIES VERSION ${KVI_VERSION} SOVERSION ${KVI_MAJOR})
INSTALL(TARGETS kvilog LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)