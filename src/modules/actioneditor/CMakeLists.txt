# CMakeLists for src/modules/actioneditor

SET(kviactioneditor_MOC_HDRS
	actioneditor.h
)

SET(kviactioneditor_SRCS
	actioneditor.cpp
	libkviactioneditor.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviactioneditor_MOC_SRCS ${kviactioneditor_MOC_HDRS})

# Create library
ADD_LIBRARY(kviactioneditor SHARED ${kviactioneditor_SRCS} ${kviactioneditor_MOC_SRCS})
#TARGET_LINK_LIBRARIES(kvilib kviactioneditor)

# Install target
SET_TARGET_PROPERTIES(kviactioneditor PROPERTIES VERSION ${KVI_VERSION} SOVERSION ${KVI_MAJOR})
INSTALL(TARGETS kviactioneditor LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)