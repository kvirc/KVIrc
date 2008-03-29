# CMakeLists for src/modules/my

SET(kvimy_MOC_HDRS
	idle.h
)

SET(kvimy_SRCS
	libkvimy.cpp
	idle_x11.cpp
	idle_mac.cpp
	idle.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvimy_MOC_SRCS ${kvimy_MOC_HDRS})

# Create library
ADD_LIBRARY(kvimy SHARED ${kvimy_SRCS} ${kvimy_MOC_SRCS})
#TARGET_LINK_LIBRARIES(kvilib kviaddon)

# Install target
SET_TARGET_PROPERTIES(kvimy PROPERTIES VERSION ${KVI_VERSION} SOVERSION ${KVI_MAJOR})
INSTALL(TARGETS kvimy LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)