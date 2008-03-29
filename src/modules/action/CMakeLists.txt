# CMakeLists for src/modules/action

SET(kviaction_SRCS
	libkviaction.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviaction_MOC_SRCS ${kviaction_MOC_HDRS})

# Create library
ADD_LIBRARY(kviaction SHARED ${kviaction_SRCS} ${kviaction_MOC_SRCS})
#TARGET_LINK_LIBRARIES(kvilib kviaction)

# Install target
SET_TARGET_PROPERTIES(kviaction PROPERTIES VERSION ${KVI_VERSION} SOVERSION ${KVI_MAJOR})
INSTALL(TARGETS kviaction LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)