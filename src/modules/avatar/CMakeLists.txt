# CMakeLists for src/modules/avatar

SET(kviavatar_MOC_HDRS
	libkviavatar.h
)

SET(kviavatar_SRCS
	libkviavatar.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviavatar_MOC_SRCS ${kviavatar_MOC_HDRS})

# Create library
ADD_LIBRARY(kviavatar SHARED ${kviavatar_SRCS} ${kviavatar_MOC_SRCS})
#TARGET_LINK_LIBRARIES(kvilib kviavatar)

# Install target
SET_TARGET_PROPERTIES(kviavatar PROPERTIES VERSION ${KVI_VERSION} SOVERSION ${KVI_MAJOR})
INSTALL(TARGETS kviavatar LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)