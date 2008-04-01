# CMakeLists for src/modules/about
SET(kviabout_MOC_HDRS
	aboutdialog.h
	libkviabout.h
)

SET(kviabout_SRCS
	libkviabout.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviabout_MOC_SRCS ${kviabout_MOC_HDRS})

# Create library
ADD_LIBRARY(kviabout SHARED ${kviabout_SRCS} ${kviabout_MOC_SRCS})
#TARGET_LINK_LIBRARIES(kvilib kviaddon)

# Install target
SET_TARGET_PROPERTIES(kviabout PROPERTIES VERSION ${KVI_VERSION} SOVERSION ${KVI_MAJOR})
INSTALL(TARGETS kviabout LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)
