# CMakeLists for src/modules/raweditor

SET(kviraweditor_MOC_HDRS
	raweditor.h
)

SET(kviraweditor_SRCS
	libkviraweditor.cpp
	raweditor.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviraweditor_MOC_SRCS ${kviraweditor_MOC_HDRS})

# Create library
ADD_LIBRARY(kviraweditor SHARED ${kviraweditor_SRCS} ${kviraweditor_MOC_SRCS})
#TARGET_LINK_LIBRARIES(kvilib kviraweditor)

# Install target
SET_TARGET_PROPERTIES(kviraweditor PROPERTIES VERSION ${KVI_VERSION} SOVERSION ${KVI_MAJOR})
INSTALL(TARGETS kviraweditor LIBRARY DESTINATION ${KVI_INSTALL_PREFIX}/share/kvirc/${KVI_VERSION_BRANCH}/modules/)